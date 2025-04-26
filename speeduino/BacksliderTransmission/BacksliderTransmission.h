#ifndef BACKSLIDERTRANSMISSION_H
#define BACKSLIDERTRANSMISSION_H

#include <Arduino.h>
#include "globals.h"

// Physical gear selector position
enum class GearSelector {
    PARK,
    REVERSE,
    NEUTRAL,
    DRIVE,
    SECOND,
    FIRST
};

// Current actual gear
enum class CurrentGear {
    PARK,
    REVERSE,
    NEUTRAL,
    FIRST,
    SECOND,
    THIRD,
    FOURTH
};

// Transmission configuration struct
struct configTransmission {
    bool enableTransmission;  // Whether transmission control is enabled
    
    // CAN configuration
    uint32_t canBaudRate;    // CAN bus baud rate
    uint32_t canAddress;     // CAN address for this module
    
    // PWM output pins for shift solenoids
    uint8_t shiftSolenoid1Pin;
    uint8_t shiftSolenoid2Pin;
    uint8_t shiftSolenoid3Pin;
    uint8_t shiftSolenoid4Pin;
    uint8_t shiftSolenoid5Pin;
    uint8_t shiftSolenoid6Pin;
    
    // CAN input aliases
    uint8_t canRPMIndex;     // Index in currentStatus.canin[] for RPM
    uint8_t canTPSIndex;     // Index in currentStatus.canin[] for TPS
    uint8_t canMAPIndex;     // Index in currentStatus.canin[] for MAP
    uint8_t canCLTIndex;     // Index in currentStatus.canin[] for Coolant Temp

    // Gear Selector from TPS Analog Input
    uint8_t gearSelector_tps[8];    // Gear selector points for 1-2 upshift
    uint8_t gearSelector_target[8];    // TPS points for 1-2 upshift

    
    // Shift points (VSS vs TPS)
    uint16_t shift1_2_up_vss[6];    // VSS points for 1-2 upshift
    uint16_t shift1_2_up_tps[6];    // TPS points for 1-2 upshift
    uint16_t shift1_2_down_vss[6];  // VSS points for 1-2 downshift
    uint16_t shift1_2_down_tps[6];  // TPS points for 1-2 downshift
    
    uint16_t shift2_3_up_vss[6];    // VSS points for 2-3 upshift
    uint16_t shift2_3_up_tps[6];    // TPS points for 2-3 upshift
    uint16_t shift2_3_down_vss[6];  // VSS points for 2-3 downshift
    uint16_t shift2_3_down_tps[6];  // TPS points for 2-3 downshift
    
    uint16_t shift3_4_up_vss[6];    // VSS points for 3-4 upshift
    uint16_t shift3_4_up_tps[6];    // TPS points for 3-4 upshift
    uint16_t shift3_4_down_vss[6];  // VSS points for 3-4 downshift
    uint16_t shift3_4_down_tps[6];  // TPS points for 3-4 downshift
};

// Global configuration
extern struct configTransmission configPageTransmission;

// Function declarations
void initTransmission();
void updateTransmission();
void sendTransmissionCAN();
void receiveTransmissionCAN();

// Helper functions for shift curves
uint16_t getShiftVSS(CurrentGear gearFrom, CurrentGear gearTo, byte tps, bool isUpshift);
byte getShiftPressure(CurrentGear gearFrom, CurrentGear gearTo, byte tps, bool isUpshift);

#endif // BACKSLIDERTRANSMISSION_H 