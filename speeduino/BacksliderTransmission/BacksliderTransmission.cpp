#include "BacksliderTransmission.h"
#include "VSSHandler.h"
#include "globals.h"  // This contains the currentStatus struct

// Debug flag for this module
static bool debugEnabled = false;

// Debug helper function
void debugTable2D(const char* tableName, table2D* table, float input) {
    if (!debugEnabled) return;
    
    Serial.print("Table ");
    Serial.print(tableName);
    Serial.print(" lookup with input ");
    Serial.print(input);
    Serial.print(" -> ");
    Serial.print(table2D_getValue(table, input));
    Serial.println();
    
    // Print table contents
    Serial.print("Table contents: ");
    for (int i = 0; i < table->xSize; i++) {
        Serial.print("(");
        Serial.print(table2D_getAxisValue(table, i));
        Serial.print(",");
        Serial.print(table2D_getRawValue(table, i));
        Serial.print(") ");
    }
    Serial.println();
}

// Global configuration
struct configTransmission configPageTransmission = {
    .enableTransmission = true,  // Enable transmission control by default
    
    // CAN configuration
    .canBaudRate = 500000,      // 500kbps CAN bus
    .canAddress = 0x7E0,        // Default CAN address
    
    // PWM output pins for shift solenoids
    .shiftSolenoid1Pin = 2,
    .shiftSolenoid2Pin = 3,
    .shiftSolenoid3Pin = 4,
    .shiftSolenoid4Pin = 5,
    .shiftSolenoid5Pin = 6,
    .shiftSolenoid6Pin = 7,
    
    // CAN input aliases
    .canRPMIndex = 0,
    .canTPSIndex = 1,
    .canMAPIndex = 2,
    .canCLTIndex = 3,
    
    // Shift points will be initialized in initTransmission()
};

// Current transmission state
static uint16_t lastShiftTime = 0;
static const uint16_t SHIFT_DELAY = 500; // 500ms shift delay

// Local pointers to currentStatus variables for better readability
static byte* const currentGear = &currentStatus.gear;
static byte* const gearSelector = &currentStatus.airConStatus;

// Helper functions for cleaner code
static inline GearSelector getGearSelector() {
    return GearSelector::DRIVE;
    //return static_cast<GearSelector>(*gearSelector);
}

static inline CurrentGear getCurrentGear() {
    return static_cast<CurrentGear>(*currentGear);
}

static inline void setCurrentGear(CurrentGear gear) {
    *currentGear = static_cast<byte>(gear);
}

// Shift curve tables
static table2D shift1_2_up_table;
static table2D shift1_2_down_table;
static table2D shift2_3_up_table;
static table2D shift2_3_down_table;
static table2D shift3_4_up_table;
static table2D shift3_4_down_table;

void initTransmission() {
    if (configPageTransmission.enableTransmission) {
        // Initialize PWM outputs for shift solenoids
        pinMode(configPageTransmission.shiftSolenoid1Pin, OUTPUT);
        pinMode(configPageTransmission.shiftSolenoid2Pin, OUTPUT);
        pinMode(configPageTransmission.shiftSolenoid3Pin, OUTPUT);
        pinMode(configPageTransmission.shiftSolenoid4Pin, OUTPUT);
        pinMode(configPageTransmission.shiftSolenoid5Pin, OUTPUT);
        pinMode(configPageTransmission.shiftSolenoid6Pin, OUTPUT);
        
        // Initialize VSS
        initVSS();
        
        // Build shift curves using flex fuel/boost tables
        // 1-2 Shift Curve
        for(byte i = 0; i < 6; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.shift1_2_up_tps[i] = configPage10.knock_window_rpms[i];
            configPageTransmission.shift1_2_down_tps[i] = configPage10.knock_window_rpms[i];
            
            // VSS points for upshift (using flexBoostAdj)
            configPageTransmission.shift1_2_up_vss[i] = configPage10.knock_window_angle[i];
            
            // VSS points for downshift (using flexFuelBins)
            configPageTransmission.shift1_2_down_vss[i] = configPage10.knock_window_dur[i];
        }
        
        // 2-3 Shift Curve fuelTempValues
        for(byte i = 0; i < 6; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.shift2_3_up_tps[i] = configPage10.fuelTempValues[i];
            configPageTransmission.shift2_3_down_tps[i] = configPage10.fuelTempValues[i];
            
            // VSS points for upshift (using flexBoostAdj)
            configPageTransmission.shift2_3_up_vss[i] = configPage10.flexBoostBins[i];
                     
            // VSS points for downshift (using flexFuelBins)
            configPageTransmission.shift2_3_down_vss[i] = configPage10.flexFuelBins[i];
        }
        
        // 3-4 Shift Curve
        for(byte i = 0; i < 6; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.shift3_4_up_tps[i] = configPage10.flexAdvBins[i];
            configPageTransmission.shift3_4_down_tps[i] = configPage10.flexAdvBins[i];
            
            // VSS points for upshift (using flexBoostAdj)
            configPageTransmission.shift3_4_up_vss[i] = configPage10.flexFuelAdj[i];
            
            // VSS points for downshift (using flexFuelBins)
            configPageTransmission.shift3_4_down_vss[i] = configPage10.flexAdvAdj[i];
        }
        
        // Construct 2D tables for shift curves
        construct2dTable(shift1_2_up_table, 6, configPageTransmission.shift1_2_up_vss, configPageTransmission.shift1_2_up_tps);
        construct2dTable(shift1_2_down_table, 6, configPageTransmission.shift1_2_down_vss, configPageTransmission.shift1_2_down_tps);
        construct2dTable(shift2_3_up_table, 6, configPageTransmission.shift2_3_up_vss, configPageTransmission.shift2_3_up_tps);
        construct2dTable(shift2_3_down_table, 6, configPageTransmission.shift2_3_down_vss, configPageTransmission.shift2_3_down_tps);
        construct2dTable(shift3_4_up_table, 6, configPageTransmission.shift3_4_up_vss, configPageTransmission.shift3_4_up_tps);
        construct2dTable(shift3_4_down_table, 6, configPageTransmission.shift3_4_down_vss, configPageTransmission.shift3_4_down_tps);
        
        // Initialize CAN interface
        // TODO: Add CAN initialization code

        setCurrentGear(CurrentGear::FIRST);
    }
}

void updateTransmission() {
    // Update VSS speed calculation
    updateVSS();
    
    // Get current engine parameters from CAN inputs
    byte currentTPS = currentStatus.canin[configPageTransmission.canTPSIndex];
    byte currentMAP = currentStatus.canin[configPageTransmission.canMAPIndex];
    byte currentCLT = currentStatus.canin[configPageTransmission.canCLTIndex];

    // Update transmission state based on current parameters
    if (getGearSelector() == GearSelector::DRIVE) {
        if(debugEnabled) {  
            Serial.println("Trying to shift!");
            Serial.print("Current VSS: ");
            Serial.println(currentStatus.vss);
            Serial.print("Current gear: ");
            Serial.print((int)getCurrentGear());
            Serial.println();
        }
        
        // Check for upshifts using the 2D tables
        switch (getCurrentGear()) {
            case CurrentGear::FIRST:
                debugTable2D("1-2 up", &shift1_2_up_table, currentTPS);
                if (currentStatus.vss > table2D_getValue(&shift1_2_up_table, currentTPS)) {
                    // Shift to 2nd
                    setCurrentGear(CurrentGear::SECOND);
                    lastShiftTime = millis();
                    if(debugEnabled) {
                        Serial.print("Shifted to gear: ");
                        Serial.print((int)getCurrentGear());
                        Serial.println();
                    }
                }
                break;
            case CurrentGear::SECOND:
                debugTable2D("2-3 up", &shift2_3_up_table, currentTPS);
                if (currentStatus.vss > table2D_getValue(&shift2_3_up_table, currentTPS)) {
                    // Shift to 3rd
                    setCurrentGear(CurrentGear::THIRD);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::THIRD:
                debugTable2D("3-4 up", &shift3_4_up_table, currentTPS);
                if (currentStatus.vss > table2D_getValue(&shift3_4_up_table, currentTPS)) {
                    // Shift to 4th
                    setCurrentGear(CurrentGear::FOURTH);
                    lastShiftTime = millis();
                }
                break;
            default:
                break;
        }
        
        // Check for downshifts
        switch (getCurrentGear()) {
            case CurrentGear::SECOND:
                debugTable2D("1-2 down", &shift1_2_down_table, currentTPS);
                if (currentStatus.vss < table2D_getValue(&shift1_2_down_table, currentTPS)) {
                    // Shift to 1st
                    setCurrentGear(CurrentGear::FIRST);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::THIRD:
                debugTable2D("2-3 down", &shift2_3_down_table, currentTPS);
                if (currentStatus.vss < table2D_getValue(&shift2_3_down_table, currentTPS)) {
                    // Shift to 2nd
                    setCurrentGear(CurrentGear::SECOND);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::FOURTH:
                debugTable2D("3-4 down", &shift3_4_down_table, currentTPS);
                if (currentStatus.vss < table2D_getValue(&shift3_4_down_table, currentTPS)) {
                    // Shift to 3rd
                    setCurrentGear(CurrentGear::THIRD);
                    lastShiftTime = millis();
                }
                break;
            default:
                break;
        }
    }
    else if (getGearSelector() == GearSelector::SECOND) {
        // Force 2nd gear
        setCurrentGear(CurrentGear::SECOND);
    }
    else if (getGearSelector() == GearSelector::FIRST) {
        // Force 1st gear
        setCurrentGear(CurrentGear::FIRST);
    }
    else if (getGearSelector() == GearSelector::REVERSE) {
        setCurrentGear(CurrentGear::REVERSE);
    }
    else if (getGearSelector() == GearSelector::NEUTRAL) {
        setCurrentGear(CurrentGear::NEUTRAL);
    }
    else if (getGearSelector() == GearSelector::PARK) {
        setCurrentGear(CurrentGear::PARK);
    }
    
    // Send current status via CAN
    sendTransmissionCAN();
    
    // Check for incoming CAN messages
    receiveTransmissionCAN();
}

void sendTransmissionCAN() {
    if (!configPageTransmission.enableTransmission) return;
    
    // Send current transmission status
    // Using CAN input values:
    // - currentStatus.vss for vehicle speed
    // - currentStatus.canin[configPageTransmission.canTPSIndex] for throttle position
    // - currentStatus.canin[configPageTransmission.canMAPIndex] for manifold pressure
    // - currentStatus.canin[configPageTransmission.canCLTIndex] for coolant temperature
    
    // TODO: Implement CAN message sending
}

void receiveTransmissionCAN() {
    if (!configPageTransmission.enableTransmission) return;
    
    // Handle incoming CAN messages
    // This will process commands from the transmission controller
    // and update the transmission state accordingly
    
    // TODO: Implement CAN message receiving
} 