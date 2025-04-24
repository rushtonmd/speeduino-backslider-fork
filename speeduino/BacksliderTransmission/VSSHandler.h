#pragma once
#include <Arduino.h>
#include "globals.h"  // For currentStatus and pinVSS
#include "config_pages.h"  // For configPage2

// Debug flag
extern bool debugEnabled;

// Function declarations
void initVSS();
void vssInterrupt();
float calculateSpeed();
void updateVSS();

// VSS Handler Configuration (if needed for tuning)
#define VSS_UPDATE_FREQUENCY_HZ 100  // Update rate for VSS calculations
#define VSS_MAX_SPEED_KMH 300       // Maximum speed in km/h
#define VSS_MIN_SPEED_KMH 0         // Minimum speed in km/h