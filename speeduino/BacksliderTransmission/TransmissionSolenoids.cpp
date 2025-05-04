#include "TransmissionSolenoids.h"

// Line Pressure Table
static table2D linePressure_table;

void initTransmissionSolenoids() {

    // Line Pressure Solenoid
    construct2dTable(linePressure_table, 6, configPage16.trans_line_pressure_pwm, configPage16.trans_line_pressure_tps);

    // Configure solenoid pins as outputs
    pinMode(configPage16.shift_solenoid_1_pin, OUTPUT);
    pinMode(configPage16.shift_solenoid_2_pin, OUTPUT);
    pinMode(configPage16.lockup_pin, OUTPUT);
    pinMode(configPage16.overrun_pin, OUTPUT);
    pinMode(configPage16.trans_line_pressure_pin, OUTPUT);
    
    // Set initial PWM values to 0
    analogWrite(configPage16.shift_solenoid_1_pin, 0);
    analogWrite(configPage16.shift_solenoid_2_pin, 0);
    analogWrite(configPage16.lockup_pin, 0);
    analogWrite(configPage16.overrun_pin, 0);
    analogWrite(configPage16.trans_line_pressure_pin, 0);
    
    // Initialize solenoid states in currentStatus
    for (int i = 0; i < 6; i++) {
        currentStatus.trans_solenoidStates[i] = 0;
    }
}

void updateTransmissionSolenoids() {
    // Get current gear from global status
    // 0=PARK, 1=REVERSE, 2=NEUTRAL, 3=4TH, 4=3RD, 5=2ND, 6=1ST
    uint8_t currentGear = currentStatus.gear;
    
    // Ensure gear index is within valid range
    if (currentGear > 6) {
        currentGear = 0; // Default to PARK if invalid
    }
    
    // Update shift solenoids 1 & 2 based on current gear
    uint8_t shiftSolenoid1PWM = configPage16.shift_solenoid_1_gear_pwm[currentGear];
    uint8_t shiftSolenoid2PWM = configPage16.shift_solenoid_2_gear_pwm[currentGear];
    
    setShiftSolenoid1(shiftSolenoid1PWM);
    setShiftSolenoid2(shiftSolenoid2PWM);
    
    // Update lockup solenoid
    if (shouldEngageLockup()) {
        setLockupSolenoid(255);
    } else {
        setLockupSolenoid(0);
    }
    
    // Update overrun solenoid
    if (shouldEngageOverrun()) {
        setOverrunSolenoid(255);
    } else {
        setOverrunSolenoid(0);
    }
    
    // Update line pressure solenoid
    uint8_t linePressurePWM = calculateLinePressure();
    setLinePressureSolenoid(linePressurePWM);
}

// Individual solenoid control functions
void setShiftSolenoid1(uint8_t pwmValue) {
    currentStatus.trans_solenoidStates[SOLENOID_SHIFT_1] = pwmValue;
    analogWrite(configPage16.shift_solenoid_1_pin, pwmValue);
}

void setShiftSolenoid2(uint8_t pwmValue) {
    currentStatus.trans_solenoidStates[SOLENOID_SHIFT_2] = pwmValue;
    analogWrite(configPage16.shift_solenoid_2_pin, pwmValue);
}

void setLockupSolenoid(uint8_t pwmValue) {
    currentStatus.trans_solenoidStates[SOLENOID_LOCKUP] = pwmValue;
    analogWrite(configPage16.lockup_pin, pwmValue);
}

void setOverrunSolenoid(uint8_t pwmValue) {
    currentStatus.trans_solenoidStates[SOLENOID_OVERRUN] = pwmValue;
    analogWrite(configPage16.overrun_pin, pwmValue);
}

void setLinePressureSolenoid(uint8_t pwmValue) {
    currentStatus.trans_solenoidStates[SOLENOID_LINE_PRESSURE] = pwmValue;
    analogWrite(configPage16.trans_line_pressure_pin, pwmValue);
}

// Get functions for current PWM values
uint8_t getShiftSolenoid1PWM() {
    return currentStatus.trans_solenoidStates[SOLENOID_SHIFT_1];
}

uint8_t getShiftSolenoid2PWM() {
    return currentStatus.trans_solenoidStates[SOLENOID_SHIFT_2];
}

uint8_t getLockupSolenoidPWM() {
    return currentStatus.trans_solenoidStates[SOLENOID_LOCKUP];
}

uint8_t getOverrunSolenoidPWM() {
    return currentStatus.trans_solenoidStates[SOLENOID_OVERRUN];
}

uint8_t getLinePressureSolenoidPWM() {
    return currentStatus.trans_solenoidStates[SOLENOID_LINE_PRESSURE];
}

// Helper functions for solenoid logic
bool shouldEngageLockup() {
    // TODO: Implement lockup logic based on speed, gear, throttle position, etc.
    // Example logic:
    // - Check if in appropriate gear (not PARK, REVERSE, or NEUTRAL)
    // - Check if vehicle speed is above minimum threshold
    // - Check if engine RPM is stable
    // - Check if not in shift transition
    
    // Simple example: only engage in 3rd or 4th gear
    uint8_t currentGear = currentStatus.gear;
    if (currentGear == 3 || currentGear == 4) {  // 3=4TH, 4=3RD
        // Add more conditions here (speed, RPM, etc.)
        return false;  // Placeholder
    }
    
    return false;
}

bool shouldEngageOverrun() {
    // TODO: Implement overrun clutch logic
    // Example logic:
    // - Check if in deceleration
    // - Check if in appropriate gear
    // - Check throttle position
    
    return true; // Placeholder
}

uint8_t calculateLinePressure() {
    // TODO: Implement line pressure calculation
    // Example logic:
    // - Base pressure on throttle position
    // - Adjust for current gear
    // - Adjust for shift events
    // - Consider engine load

    uint8_t linePressure = table2D_getValue(&linePressure_table, currentStatus.canTPS);
    
    return linePressure; // Placeholder - mid-range pressure
}