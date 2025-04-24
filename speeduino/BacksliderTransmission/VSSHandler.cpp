#include "VSSHandler.h"
#include <Arduino.h>
#include "globals.h"  // For pinVSS
#include "config_pages.h"  // For configPage2

// Constants
const unsigned long STOPPED_TIMEOUT = 3000; // 3 seconds without pulses = stopped
const unsigned long MIN_PULSE_INTERVAL = 50; // Minimum 50us between pulses (debounce)
const float LOW_SPEED_THRESHOLD = 10.0; // km/h
const float ULTRA_LOW_SPEED_THRESHOLD = 3.0; // km/h
const float MAX_ACCELERATION = 50.0; // Max acceleration in km/h per second
const float MAX_DECELERATION = 100.0; // Max deceleration in km/h per second

// State variables
static volatile unsigned long lastPulseTimeMicros = 0;
static volatile unsigned long pulseInterval = 0;
static volatile bool newPulseAvailable = false;
static float currentSpeed = 0.0;
static float smoothedSpeed = 0.0;
static unsigned long lastUpdateTime = 0;
static unsigned long lastValidPulseTime = 0;
static float speedBuffer[20] = {0}; // Increased buffer for more smoothing at low speeds
static uint8_t bufferIndex = 0;
static bool bufferFull = false;
static float exponentialAverage = 0.0; // Added for extra smoothing

// Advanced state tracking
static float acceleration = 0.0;
static float previousSpeed = 0.0;
static float predictedSpeed = 0.0;
static bool isDecelerating = false;

// External declarations
extern bool debugEnabled;

void initVSS() {
    // Clean up any existing interrupts on the VSS pin
    detachInterrupt(digitalPinToInterrupt(pinVSS));
    
    // Set up VSS interrupt
    pinMode(pinVSS, INPUT);
    
    // Initialize variables
    lastPulseTimeMicros = 0;
    lastValidPulseTime = 0;
    currentSpeed = 0.0;
    smoothedSpeed = 0.0;
    bufferIndex = 0;
    bufferFull = false;
    
    // Attach interrupt on rising edge
    attachInterrupt(digitalPinToInterrupt(pinVSS), vssInterrupt, RISING);
}

void vssInterrupt() {
    unsigned long currentTimeMicros = micros();
    unsigned long interval = currentTimeMicros - lastPulseTimeMicros;
    
    // Only process if interval is reasonable (prevents noise)
    if (interval >= MIN_PULSE_INTERVAL) {
        pulseInterval = interval;
        lastPulseTimeMicros = currentTimeMicros;
        lastValidPulseTime = millis();
        newPulseAvailable = true;
    }
}

// Advanced smoothing function with adaptive behavior
static float adaptiveSmooth(float input, float current, float timeDelta) {
    // Calculate smoothing factor based on configPage2.vssSmoothing (0-255)
    float baseSmoothing = (float)configPage2.vssSmoothing / 255.0;
    
    // Adjust smoothing based on speed range and rate of change
    float speedDiff = fabs(input - current);
    float adaptiveFactor = baseSmoothing;
    
    // Less smoothing for large changes (quick response)
    if (speedDiff > 10.0) {
        adaptiveFactor *= 0.7;
    }
    // More smoothing for small changes (stability)
    else if (speedDiff < 1.0) {
        adaptiveFactor = min(baseSmoothing * 1.2, 0.98);
    }
    
    // Special handling for very low speeds - use maximum smoothing
    if (current < LOW_SPEED_THRESHOLD && input < LOW_SPEED_THRESHOLD) {
        adaptiveFactor = min(baseSmoothing * 1.5, 0.99);
    }
    
    // Consider time factor
    float timeAdjustment = 1.0 - (timeDelta / 2000.0);
    timeAdjustment = constrain(timeAdjustment, 0.5, 1.0);
    adaptiveFactor = adaptiveFactor * timeAdjustment;
    
    // Allow more aggressive smoothing for high values
    adaptiveFactor = constrain(adaptiveFactor, 0.05, 0.99);
    
    return current + (input - current) * (1.0 - adaptiveFactor);
}

// Predictive speed calculation for low speeds
static float predictiveCalculation(float currentSpeed, float timeSinceLastPulse) {
    if (timeSinceLastPulse < 100) return currentSpeed;
    
    // Use acceleration trend to predict speed
    float predictedDeceleration = acceleration < 0 ? acceleration : -5.0; // Default deceleration
    float predictedSpeed = currentSpeed + (predictedDeceleration * timeSinceLastPulse / 1000.0);
    
    return max(predictedSpeed, 0.0);
}

float calculateSpeed() {
    unsigned long currentTime = millis();
    float timeDelta = (currentTime - lastUpdateTime) / 1000.0; // Convert to seconds
    if (timeDelta <= 0) timeDelta = 0.001;
    
    lastUpdateTime = currentTime;
    
    // Check if we're stopped (no pulses for a while)
    if (lastValidPulseTime > 0 && (currentTime - lastValidPulseTime) > STOPPED_TIMEOUT) {
        currentSpeed = 0.0;
        acceleration = 0.0;
        isDecelerating = false;
        return 0.0;
    }
    
    // Process new pulse if available
    if (newPulseAvailable) {
        newPulseAvailable = false;
        
        if (pulseInterval > 0) {
            // Convert pulse interval to speed (km/h)
            float hz = 1000000.0 / pulseInterval;
            float rawSpeed = (hz * 3600.0) / configPage2.vssPulsesPerKm;
            
            // Limit maximum rate of change
            float maxChange = isDecelerating ? MAX_DECELERATION : MAX_ACCELERATION;
            maxChange *= timeDelta;
            
            if (rawSpeed > currentSpeed + maxChange) {
                rawSpeed = currentSpeed + maxChange;
            } else if (rawSpeed < currentSpeed - maxChange) {
                rawSpeed = currentSpeed - maxChange;
            }
            
            // Update speed buffer for additional smoothing
            speedBuffer[bufferIndex] = rawSpeed;
            bufferIndex = (bufferIndex + 1) % 20;
            if (bufferIndex == 0) bufferFull = true;
            
            // Calculate weighted average from buffer for ultra-low speeds
            if (rawSpeed < ULTRA_LOW_SPEED_THRESHOLD) {
                float sum = 0;
                float weightSum = 0;
                int count = bufferFull ? 20 : bufferIndex;
                
                // Apply weighted average - more recent samples have more weight
                for (int i = 0; i < count; i++) {
                    int idx = (bufferIndex - 1 - i + 20) % 20;
                    float weight = 1.0 + (float)i / count; // Recent samples weighted more
                    sum += speedBuffer[idx] * weight;
                    weightSum += weight;
                }
                currentSpeed = weightSum > 0 ? sum / weightSum : 0;
                
                // Apply exponential smoothing for ultra-smooth output
                float alpha = 0.1; // Lower alpha = more smoothing
                if (configPage2.vssSmoothing > 200) {
                    alpha = 0.05; // Even more smoothing for high smoothing values
                }
                exponentialAverage = alpha * currentSpeed + (1.0 - alpha) * exponentialAverage;
                currentSpeed = exponentialAverage;
            } else {
                currentSpeed = rawSpeed;
                exponentialAverage = currentSpeed; // Reset for next time
            }
            
            // Calculate acceleration
            acceleration = (currentSpeed - previousSpeed) / timeDelta;
            previousSpeed = currentSpeed;
            isDecelerating = acceleration < -0.5;
        }
    } else {
        // No new pulse - handle low speed prediction
        unsigned long timeSinceLastPulse = currentTime - lastValidPulseTime;
        
        if (currentSpeed > LOW_SPEED_THRESHOLD) {
            // Use deceleration prediction for higher speeds
            predictedSpeed = predictiveCalculation(currentSpeed, timeSinceLastPulse);
            currentSpeed = max(predictedSpeed, 0.0);
        } else if (currentSpeed > 0) {
            // For very low speeds, use time-based calculation
            float maxSpeedFromInterval = (3600000.0 / timeSinceLastPulse) / configPage2.vssPulsesPerKm;
            currentSpeed = min(currentSpeed, maxSpeedFromInterval);
            
            // Apply gentle deceleration
            if (timeSinceLastPulse > 500) {
                currentSpeed *= (1.0 - (timeSinceLastPulse - 500) / 5000.0);
                currentSpeed = max(currentSpeed, 0.0);
            }
        }
    }
    
    return currentSpeed;
}

void updateVSS() {
    float rawSpeed = calculateSpeed();
    
    // Apply adaptive smoothing
    float timeDelta = (millis() - lastUpdateTime) / 1000.0;
    smoothedSpeed = adaptiveSmooth(rawSpeed, smoothedSpeed, timeDelta);
    
    // Ensure non-negative speed
    smoothedSpeed = max(smoothedSpeed, 0.0);
    
    // Apply final limits and update global variable
    currentStatus.vss = constrain((uint16_t)smoothedSpeed, 0, 255);
    
    // Debug output
    if(debugEnabled) {
        static unsigned long lastDebugTime = 0;
        if (millis() - lastDebugTime > 100) { // Limit debug output frequency
            Serial.print("Raw: ");
            Serial.print(rawSpeed, 1);
            Serial.print(" km/h, Smoothed: ");
            Serial.print(smoothedSpeed, 1);
            Serial.print(" km/h, Accel: ");
            Serial.print(acceleration, 1);
            Serial.print(" km/h/s, Smoothing: ");
            Serial.print(configPage2.vssSmoothing);
            Serial.print("/255 (");
            Serial.print((configPage2.vssSmoothing * 100) / 255);
            Serial.println("%)");
            lastDebugTime = millis();
        }
    }
}