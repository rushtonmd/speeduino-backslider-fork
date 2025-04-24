#ifndef RE4R01A_H
#define RE4R01A_H

#include <Arduino.h>

// Solenoid States
enum class SolenoidState {
    OFF = 0,
    ON = 1
};

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
    static const uint8_t SHIFT_SOLENOID_C = 3;  // Pin 3
    static const uint8_t LOCKUP_SOLENOID = 4;   // Pin 4
    static const uint8_t LINE_PRESSURE_SOLENOID = 5;  // Pin 5
    static const uint8_t ACCUMULATOR_SOLENOID = 6;    // Pin 6
};

// Sensor Pin Definitions
struct SensorPins {
    static const uint8_t INPUT_SPEED_SENSOR_POS = 9;   // Pin 9
    static const uint8_t INPUT_SPEED_SENSOR_NEG = 10;  // Pin 10
    static const uint8_t OUTPUT_SPEED_SENSOR_POS = 11; // Pin 11
    static const uint8_t OUTPUT_SPEED_SENSOR_NEG = 12; // Pin 12
    static const uint8_t TEMP_SENSOR = 13;             // Pin 13
};

// PWM Control Parameters
struct PWMControl {
    static const uint16_t FREQUENCY = 292;  // Hz
    static const uint8_t MIN_DUTY = 0;      // %
    static const uint8_t MAX_DUTY = 100;    // %
};

// Shift Points (km/h)
struct ShiftPoints {
    // 1-2 Shift
    static const uint8_t LIGHT_1_2 = 20;
    static const uint8_t MEDIUM_1_2 = 30;
    static const uint8_t HEAVY_1_2 = 40;

    // 2-3 Shift
    static const uint8_t LIGHT_2_3 = 35;
    static const uint8_t MEDIUM_2_3 = 50;
    static const uint8_t HEAVY_2_3 = 65;

    // 3-4 Shift
    static const uint8_t LIGHT_3_4 = 50;
    static const uint8_t MEDIUM_3_4 = 70;
    static const uint8_t HEAVY_3_4 = 90;
};

// Lockup Control
struct LockupControl {
    static const uint8_t ENGAGE_SPEED = 65;     // km/h
    static const uint8_t DISENGAGE_SPEED = 60;  // km/h
    static const uint8_t MIN_TPS = 15;          // %
    static const uint8_t MAX_TPS = 85;          // %
};

// Sensor Specifications
struct SensorSpecs {
    static const uint8_t PULSES_PER_REV = 20;
    static const uint16_t TEMP_SENSOR_RESISTANCE = 10000;  // 10kΩ at 25°C
    static const int8_t TEMP_MIN = -40;  // °C
    static const uint8_t TEMP_MAX = 150; // °C
};

// Gear Ratios
struct GearRatios {
    static const float FIRST = 2.785f;
    static const float SECOND = 1.545f;
    static const float THIRD = 1.000f;
    static const float FOURTH = 0.694f;
    static const float REVERSE = 2.272f;
};

// Safety Parameters
struct SafetyParams {
    static const uint8_t TEMP_LOCKUP_DISABLE = 130;  // °C
    static const uint8_t TEMP_PRESSURE_REDUCE = 140; // °C
    static const uint8_t BASE_PRESSURE = 60;         // PSI
    static const uint8_t MAX_PRESSURE = 120;         // PSI
    static const uint8_t MIN_PRESSURE = 30;          // PSI
};

// Timing Parameters (seconds)
struct TimingParams {
    static const float BASE_SHIFT_TIME = 0.5f;
    static const float MAX_SHIFT_TIME = 1.0f;
    static const float MIN_SHIFT_TIME = 0.2f;
    static const float LOCKUP_ENGAGE_DELAY = 0.2f;
    static const float LOCKUP_DISENGAGE_DELAY = 0.1f;
    static const float MIN_LOCKUP_TIME = 1.0f;
};

// Solenoid State Table for Each Gear
struct SolenoidStates {
    static const SolenoidState FIRST_GEAR[3];
    static const SolenoidState SECOND_GEAR[3];
    static const SolenoidState THIRD_GEAR[3];
    static const SolenoidState FOURTH_GEAR[3];
};

#endif // RE4R01A_H 