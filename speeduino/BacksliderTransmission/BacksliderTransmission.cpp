#include "BacksliderTransmission.h"
#include "VSSHandler.h"
#include "TemperatureSensor.h"
#include "PaddleShifter.h"
#include "globals.h"  // This contains the currentStatus struct
#include "pages.h"    // For getPageValue
#include <EEPROM.h>   // For EEPROM access
// #include "CANHandler.h"  // For CAN functionality

// Debug flag for this module
bool debugEnabled = false;

// Debug helper function
void debugTable2D(const char* tableName, table2D* table, float input) {
    // if (!debugEnabled) return;
    
    // Serial.print("Table ");
    // Serial.print(tableName);
    // Serial.print(" and size of ");
    // Serial.print(table->xSize);
    // Serial.print(" lookup with input ");
    // Serial.print(input);
    // Serial.print(" -> ");
    // Serial.print(table2D_getValue(table, input));
    // Serial.println();
    
    // // Print table contents
    // Serial.print("Table contents: ");
    // for (int i = 0; i < table->xSize; i++) {
    //     Serial.print("(");
    //     Serial.print(table2D_getAxisValue(table, i));
    //     Serial.print(",");
    //     Serial.print(table2D_getRawValue(table, i));
    //     Serial.print(") ");
    // }
    // Serial.println();
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
static const uint16_t SHIFT_DELAY = 500; // 500ms shift delay

// Local pointers to currentStatus variables for better readability
static byte* const currentGear = &currentStatus.gear;
//static byte* const gearSelector = &currentStatus.gearSelector;

// Gear selector tables
static table2D gearSelector_table;

// Shift curve tables
static table2D shift1_2_up_table;
static table2D shift1_2_down_table;
static table2D shift2_3_up_table;
static table2D shift2_3_down_table;
static table2D shift3_4_up_table;
static table2D shift3_4_down_table;

// Temperature sensor instance
static TempSensor transTempSensor;

// Gear selector functions
// inline GearSelector getGearSelector() {
//     return static_cast<GearSelector>(currentStatus.gearSelectorChosenGear);
// }

// Function to read and output EEPROM values
void dumpEEPROM(uint16_t startAddr, uint16_t endAddr) {
    // if (!debugEnabled) return;
    
    // Serial.println("\nEEPROM Dump:");
    // Serial.print("Start Address: 0x");
    // Serial.print(startAddr, HEX);
    // Serial.print(" End Address: 0x");
    // Serial.println(endAddr, HEX);
    
    // for (uint16_t addr = startAddr; addr <= endAddr; addr++) {
    //     if ((addr - startAddr) % 16 == 0) {
    //         Serial.println();
    //         Serial.print("0x");
    //         Serial.print(addr, HEX);
    //         Serial.print(": ");
    //     }
        
    //     byte value = EEPROM.read(addr);
    //     if (value < 16) Serial.print("0"); // Pad single digit hex values
    //     Serial.print(value, HEX);
    //     Serial.print(" ");
    // }
    // Serial.println("\n");
}

void setTransmissionPins() {
    // Set all solenoid pins to OUTPUT mode
    pinMode(configPageTransmission.shiftSolenoid1Pin, OUTPUT);
    pinMode(configPageTransmission.shiftSolenoid2Pin, OUTPUT);
    pinMode(configPageTransmission.shiftSolenoid3Pin, OUTPUT);
    pinMode(configPageTransmission.shiftSolenoid4Pin, OUTPUT);
    pinMode(configPageTransmission.shiftSolenoid5Pin, OUTPUT);
    pinMode(configPageTransmission.shiftSolenoid6Pin, OUTPUT);
    
    // Initialize all solenoids to OFF state
    digitalWrite(configPageTransmission.shiftSolenoid1Pin, LOW);
    digitalWrite(configPageTransmission.shiftSolenoid2Pin, LOW);
    digitalWrite(configPageTransmission.shiftSolenoid3Pin, LOW);
    digitalWrite(configPageTransmission.shiftSolenoid4Pin, LOW);
    digitalWrite(configPageTransmission.shiftSolenoid5Pin, LOW);
    digitalWrite(configPageTransmission.shiftSolenoid6Pin, LOW);

    // Additional pins (hardcoded for now)
    // Gear selector input (analog)
    //pinMode(configPage16.trans_temp_sensor_pin, INPUT);
    
    // Transmission coolant temp sensor input (analog)
    pinMode(configPage16.shiftSelector_adc_pin, INPUT);
    
    // Shift up button input (digital with pullup)
    pinMode(40, INPUT);
    
    // Shift down button input (digital with pullup)
    pinMode(41, INPUT);
}

void printTempSensorDebug(TempSensor* sensor) {
  // Read raw ADC value
  uint16_t adcValue = readRawTemp(sensor);
  
  // Calculate voltage
  float voltage = (adcValue * 5.0) / 1023.0;
  
  // Calculate resistance
  float resistance = sensor->r1 * (voltage / (5.0 - voltage));
  
  // Calculate temperature in Kelvin
  float tKelvin = 1.0 / ((1.0 / sensor->t0) + (1.0 / sensor->bValue) * log(resistance / sensor->r0));
  
  // Convert to Celsius
  float tCelsius = tKelvin - 273.15;

  int16_t rawTemp1 = configPage16.trans_temp_sensor_calibration_temp[0];
  
  // Print all values
  Serial.println(F("Temperature Sensor Debug Information:"));

  // Debug print all possible interpretations
Serial.print(F("Resistance1 full value: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[0]);
Serial.print(F("As float: ")); Serial.println((float)configPage16.trans_temp_sensor_calibration_resistance[0]);
Serial.print(F("Divided by 10: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[0] / 10.0);
Serial.print(F("Divided by 100: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[0] / 100.0);
Serial.print(F("High byte: ")); Serial.println((configPage16.trans_temp_sensor_calibration_resistance[0] >> 8) & 0xFF);
Serial.print(F("Low byte: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[0] & 0xFF);
  // Add this debug print right after initializing your sensor
Serial.println(F("Initialization Parameters:"));
Serial.print(F("Pin: ")); Serial.println(configPage16.trans_temp_sensor_pin);
Serial.print(F("Pullup: ")); Serial.println(configPage16.trans_temp_sensor_pullup_ohms);
Serial.print(F("Temp Point 1: ")); Serial.println(rawTemp1);
Serial.print(F("Temp Point 1: ")); Serial.println((float)rawTemp1);
Serial.print(F("Resistance Point 1: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[0]);
Serial.print(F("Temp Point 2: ")); Serial.println(configPage16.trans_temp_sensor_calibration_temp[1]);
Serial.print(F("Resistance Point 2: ")); Serial.println(configPage16.trans_temp_sensor_calibration_resistance[1]);
  Serial.print(F("ADC Pin: ")); Serial.println(sensor->pin);
  Serial.print(F("Fixed Resistor: ")); Serial.print(sensor->r1); Serial.println(F(" ohms"));
  Serial.print(F("B-Value: ")); Serial.println(sensor->bValue);
  Serial.print(F("Reference T0: ")); Serial.print(sensor->t0 - 273.15); Serial.println(F("°C ("));
  Serial.print(sensor->t0); Serial.println(F("K)"));
  Serial.print(F("Reference R0: ")); Serial.print(sensor->r0); Serial.println(F(" ohms"));
  Serial.println(F("----- Measurements -----"));
  Serial.print(F("ADC Raw Value: ")); Serial.println(adcValue);
  Serial.print(F("Voltage: ")); Serial.print(voltage); Serial.println(F("V"));
  Serial.print(F("Thermistor Resistance: ")); Serial.print(resistance); Serial.println(F(" ohms"));
  Serial.print(F("Temperature: ")); Serial.print(tCelsius); Serial.println(F("°C"));
  Serial.println(F("------------------------"));
}

void initTransmission() {
    if (configPageTransmission.enableTransmission) {
        // Initialize all transmission pins
        setTransmissionPins();

        // Initialize VSS
        initVSS();

        // Initialize temperature sensor
        initTempSensor(
            &transTempSensor, 
            configPage16.trans_temp_sensor_pin, 
            configPage16.trans_temp_sensor_pullup_ohms, 
            configPage16.trans_temp_sensor_calibration_temp[0], 
            configPage16.trans_temp_sensor_calibration_resistance[0], 
            configPage16.trans_temp_sensor_calibration_temp[1], 
            configPage16.trans_temp_sensor_calibration_resistance[1]);

        // Initialize paddle shifters
        initPaddleShifters();

        // Calculate gear selector value from TPS analog input  
        for(byte i = 0; i < 8; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.gearSelector_tps[i] = getPageValue(15, 64 + i);
            //configPageTransmission.gearSelector_tps[i] = configPage15.rpmBinsDutyLookup[i];
            configPageTransmission.gearSelector_target[i] = getPageValue(15, 72 + i);
            //configPageTransmission.gearSelector_target[i] = configPage15.loadBinsDutyLookup[i];

            // VSS points for downshift (using flexFuelBins)
            //configPageTransmission.gearSelector_tps[i] = getPageValue(15, 64 + i);
        }
        
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
        
        // Construct 2D tables for gear selector
        construct2dTable(gearSelector_table, 8, configPageTransmission.gearSelector_target, configPageTransmission.gearSelector_tps);


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
    // Update paddle shifter state
    updatePaddleShifters();

    // Update VSS speed calculation
    updateVSS();

    // TEST
    float lastTemp = calculateTemperature(&transTempSensor);
    currentStatus.transTemp = lastTemp;

    // Get current engine parameters from CAN inputs
    byte currentTPS = currentStatus.canin[configPageTransmission.canTPSIndex];
    byte currentMAP = currentStatus.canin[configPageTransmission.canMAPIndex];
    byte currentCLT = currentStatus.canin[configPageTransmission.canCLTIndex];
    byte currentGearSelector = currentStatus.tpsADC;

    // Update transmission state based on current parameters
    if (getGearSelector() == GearSelector::DRIVE) {
        // Check for upshifts using the 2D tables
        switch (getCurrentGear()) {
            case CurrentGear::FIRST:
                debugTable2D("1-2 up", &shift1_2_up_table, currentTPS);
                if (currentStatus.vss > table2D_getValue(&shift1_2_up_table, currentTPS)) {
                    // Shift to 2nd
                    setCurrentGear(CurrentGear::SECOND);
                    lastShiftTime = millis();
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
    //if (!configPageTransmission.enableTransmission) return;
    
    // Process any incoming CAN messages
    //transCAN_ProcessMessages();
    
    // The actual message processing is handled in CANHandler.cpp's transCAN_ProcessMessages()
    // where we can add specific message handling logic for different message IDs
    // For example:
    // - Gear position updates
    // - Temperature readings
    // - Pressure readings
    // - Error codes
    // - Status updates
}

void setGearSelector(GearSelector gear) {
    // Update the gear selector position in the current status
    currentStatus.gearSelectorChosenGear = static_cast<uint8_t>(gear);
} 