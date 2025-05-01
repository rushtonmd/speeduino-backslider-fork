#ifndef RE4R01A_H
#define RE4R01A_H

#include <Arduino.h>

// Gear States
enum class GearState {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3,
    FOURTH = 4,
    REVERSE = 5,
    NEUTRAL = 6,
    PARK = 7
};

// Solenoid Pin Definitions
struct SolenoidPins {
    static const uint8_t SHIFT_SOLENOID_A = 1;  // Pin 1
    static const uint8_t SHIFT_SOLENOID_B = 2;  // Pin 2
    static const uint8_t LOCKUP_SOLENOID = 3;   // Pin 3
    static const uint8_t LINE_PRESSURE_SOLENOID = 4;  // Pin 4
    static const uint8_t OVERRUN_SOLENOID = 5;    // Pin 5
};

// PWM Control Parameters
struct PWMControl {
    static const uint16_t FREQUENCY = 292;  // Hz
    static const uint8_t MIN_DUTY = 0;      // %
    static const uint8_t MAX_DUTY = 100;    // %
    
    // Default duty cycles for each solenoid
    static const uint8_t SHIFT_A_DEFAULT = 50;  // %
    static const uint8_t SHIFT_B_DEFAULT = 50;  // %
    static const uint8_t LOCKUP_DEFAULT = 50;   // %
    static const uint8_t LINE_PRESSURE_DEFAULT = 50;  // %
    static const uint8_t OVERRUN_DEFAULT = 50;  // %
};

// Core transmission control functions
void setGear(GearState gear);
void updatePWMValues(uint8_t shiftA, uint8_t shiftB, uint8_t lockup, uint8_t linePressure, uint8_t overrun);

#endif // RE4R01A_H 