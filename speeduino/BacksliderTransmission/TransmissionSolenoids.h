#ifndef TRANSMISSION_SOLENOIDS_H
#define TRANSMISSION_SOLENOIDS_H

#include <Arduino.h>
#include "globals.h"

/*
 * Transmission Solenoids Control Module
 * -------------------------------------
 * This module manages all solenoid outputs for the transmission system.
 * 
 * Solenoids:
 * 1. Shift Solenoid 1 - Controls hydraulic pressure for gear shifts
 * 2. Shift Solenoid 2 - Controls hydraulic pressure for gear shifts
 * 3. Lockup Solenoid - Controls torque converter lockup
 * 4. Overrun Solenoid - Controls overrun clutch engagement
 * 5. Line Pressure Solenoid - Controls main line pressure
 * 
 * Operation:
 * - Each solenoid uses PWM control (0-255)
 * - Shift solenoids 1 & 2 have different PWM values for each gear
 * - Values are stored in arrays indexed by gear position
 * - Uses currentStatus.currentGear for gear position
 * - Solenoid states are stored in currentStatus.trans_solenoidStates[]
 * 
 * Configuration:
 * - PWM values are set through the UI and stored in configPage16
 * - Arrays store PWM values for each gear position
 * - Gear indices: 0=PARK, 1=REVERSE, 2=NEUTRAL, 3=4TH, 4=3RD, 5=2ND, 6=1ST
 */

// Solenoid array indices for currentStatus.trans_solenoidStates[]
#define SOLENOID_SHIFT_1      0
#define SOLENOID_SHIFT_2      1
#define SOLENOID_LOCKUP       2
#define SOLENOID_OVERRUN      3
#define SOLENOID_LINE_PRESSURE 4
// Index 5 is reserved for future use

// Function declarations
void initTransmissionSolenoids();
void updateTransmissionSolenoids();

// Solenoid control functions
void setShiftSolenoid1(uint8_t pwmValue);
void setShiftSolenoid2(uint8_t pwmValue);
void setLockupSolenoid(uint8_t pwmValue);
void setOverrunSolenoid(uint8_t pwmValue);
void setLinePressureSolenoid(uint8_t pwmValue);

// Get current PWM values (from currentStatus.trans_solenoidStates[])
uint8_t getShiftSolenoid1PWM();
uint8_t getShiftSolenoid2PWM();
uint8_t getLockupSolenoidPWM();
uint8_t getOverrunSolenoidPWM();
uint8_t getLinePressureSolenoidPWM();

// Helper functions
bool shouldEngageLockup();
bool shouldEngageOverrun();
uint8_t calculateLinePressure();

#endif // TRANSMISSION_SOLENOIDS_H