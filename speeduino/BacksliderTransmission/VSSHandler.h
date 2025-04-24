#ifndef VSS_HANDLER_H
#define VSS_HANDLER_H
#include <Arduino.h>

// Debug flag
extern bool debugEnabled;

// VSS configuration
static const float VSS_PULSES_PER_KM = 1242.0; // Calibrated for 33 Hz = 60 km/h
static const unsigned long MIN_PULSE_INTERVAL = 10000; // Minimum pulse interval in microseconds to prevent noise
static const float SMOOTHING_FACTOR = 0.3; // Adjust this value between 0.0 and 1.0 (lower = smoother)
static const float LOW_SPEED_THRESHOLD = 8.0; // Hz or equivalent speed (~23 kph)

// Circular buffer for pulse times
static const int PULSE_BUFFER_SIZE = 10; // Store last 10 pulse times
static volatile unsigned long pulseTimes[PULSE_BUFFER_SIZE];
static volatile int pulseBufferIndex = 0;
static volatile bool pulseBufferFull = false;

// VSS state variables
static volatile unsigned long lastVssTime = 0;
static volatile bool vssInterruptInProgress = false;
static float smoothedSpeed = 0.0; // For storing the smoothed speed value
static volatile unsigned long lastValidPulseTime = 0; // For low-speed calculations
static float lowSpeedValue = 0.0; // For storing time-based low speed value

// Function declarations
void initVSS();
void vssInterrupt();
float calculateSpeed();
void updateVSS();

#endif // VSS_HANDLER_H