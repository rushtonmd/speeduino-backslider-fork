#include "VSSHandler.h"
#include "globals.h"

void initVSS() {
  // Clean up any existing interrupts on pin 21
  detachInterrupt(digitalPinToInterrupt(21));
  // Set up VSS interrupt with hard coded pin 21
  pinMode(21, INPUT);
  // Attach interrupt on rising edge
  attachInterrupt(21, vssInterrupt, RISING);
}

void vssInterrupt() {
  // Prevent re-entrancy
  if (vssInterruptInProgress) return;
  vssInterruptInProgress = true;
  
  unsigned long currentTime = micros();
  unsigned long pulseInterval = currentTime - lastVssTime;
  
  // Only store pulse if interval is reasonable (prevents noise)
  if (pulseInterval >= MIN_PULSE_INTERVAL) {
    pulseTimes[pulseBufferIndex] = currentTime;
    pulseBufferIndex = (pulseBufferIndex + 1) % PULSE_BUFFER_SIZE;
    if (pulseBufferIndex == 0) {
      pulseBufferFull = true;
    }
    
    // Update the last valid pulse time for low-speed calculations
    lastValidPulseTime = millis();
  }
  
  lastVssTime = currentTime;
  vssInterruptInProgress = false;
}

float calculateSpeed() {
  float totalSpeed = 0.0;
  int validPulses = 0;
  int usedElements = pulseBufferFull ? PULSE_BUFFER_SIZE : pulseBufferIndex;
  
  if (usedElements <= 1) {
    return 0.0; // Not enough data points
  }
  
  // Calculate average speed from pulse intervals
  for (int i = 0; i < usedElements; i++) {
    int nextIndex = (i + 1) % usedElements;
    
    // Handle wraparound correctly by considering micros() overflow
    unsigned long interval;
    if (pulseTimes[nextIndex] >= pulseTimes[i]) {
      interval = pulseTimes[nextIndex] - pulseTimes[i];
    } else {
      // Handle micros() overflow (rare but possible)
      continue; // Skip this interval if it appears invalid
    }
    
    if (interval >= MIN_PULSE_INTERVAL) {
      // Convert interval to frequency (Hz)
      float frequency = 1000000.0 / interval; // Convert microseconds to Hz
      // Convert frequency to speed (km/h)
      float pulseSpeed = (frequency * 3600.0) / VSS_PULSES_PER_KM;
      totalSpeed += pulseSpeed;
      validPulses++;
    }
  }
  
  // Return the average speed if we have valid pulses
  if (validPulses > 0) {
    return totalSpeed / validPulses;
  }
  
  return 0.0;
}

void updateVSS() {
  float rawSpeed = calculateSpeed();
  unsigned long currentMillis = millis();
  
  // If we're in low-speed range, use time-based calculation
  if (rawSpeed < LOW_SPEED_THRESHOLD) {
    if (lastValidPulseTime > 0) {
      unsigned long timeSinceLastPulse = currentMillis - lastValidPulseTime;
      
      // If we haven't seen a pulse in a while but not too long (still moving)
      if (timeSinceLastPulse > 100 && timeSinceLastPulse < 5000) {
        // Convert to km/h: (1 pulse / interval_ms) * 1000ms/s * 3600s/h / pulses_per_km
        lowSpeedValue = (3600000.0 / timeSinceLastPulse) / VSS_PULSES_PER_KM;
        
        // Use this value if it's in our low speed range
        if (lowSpeedValue < LOW_SPEED_THRESHOLD) {
          rawSpeed = lowSpeedValue;
        }
      } else if (timeSinceLastPulse >= 5000) {
        // If no pulse for 5 seconds, assume stopped
        rawSpeed = 0;
      }
    }
  }
  
  // Apply exponential moving average smoothing
  smoothedSpeed = (SMOOTHING_FACTOR * rawSpeed) + ((1.0 - SMOOTHING_FACTOR) * smoothedSpeed);
  
  // Cap maximum speed to prevent overflow
  if (smoothedSpeed > 255) smoothedSpeed = 255;
  currentStatus.vss = (uint16_t)smoothedSpeed;
  
  // Debug output
  if(debugEnabled) {
    // Get count of valid pulses for debugging
    int validPulses = 0;
    int usedElements = pulseBufferFull ? PULSE_BUFFER_SIZE : pulseBufferIndex;
    
    for (int i = 0; i < usedElements - 1; i++) {
      int nextIndex = (i + 1) % usedElements;
      unsigned long interval = pulseTimes[nextIndex] - pulseTimes[i];
      if (interval >= MIN_PULSE_INTERVAL) {
        validPulses++;
      }
    }
    
    Serial.print("Raw Speed: ");
    Serial.print(rawSpeed);
    Serial.print(" km/h, Smoothed: ");
    Serial.print(smoothedSpeed);
    Serial.print(" km/h, Valid Pulses: ");
    Serial.print(validPulses);
    Serial.print(", Time since last pulse: ");
    Serial.print(currentMillis - lastValidPulseTime);
    Serial.println(" ms");
  }
}