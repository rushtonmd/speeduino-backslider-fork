#include "BacksliderTransmission.h"
#include "globals.h"  // This contains the currentStatus struct

// VSS interrupt handling
static volatile unsigned long lastVssTime = 0;
static volatile unsigned long vssPulseCount = 0;
static volatile bool vssInterruptInProgress = false;
static const float VSS_PULSES_PER_KM = 5000.0; // 5000 pulses per kilometer
static const unsigned long MIN_PULSE_INTERVAL = 1000; // Minimum pulse interval in microseconds to prevent noise

// VSS interrupt handler
void vssInterrupt() {
    // Prevent re-entrancy
    if (vssInterruptInProgress) return;
    vssInterruptInProgress = true;
    
    unsigned long currentTime = micros();
    unsigned long pulseInterval = currentTime - lastVssTime;
    lastVssTime = currentTime;
    vssPulseCount++;
    
    // Debug output
    //Serial.print("VSS Interrupt! Count: ");
    //Serial.print(vssPulseCount);
    //Serial.print(" Interval: ");
    //Serial.print(pulseInterval);
    //Serial.print(" Pin state: ");
    //Serial.println(digitalRead(21));
    
    // Calculate speed in km/h
    // Formula: (3600 * 1000000) / (pulseInterval * VSS_PULSES_PER_KM)
    // Where:
    // - 3600 is seconds per hour
    // - 1000000 converts microseconds to seconds
    // - VSS_PULSES_PER_KM is the number of pulses per kilometer
    // - pulseInterval is the time between pulses in microseconds
    if (pulseInterval > MIN_PULSE_INTERVAL) { // Add minimum interval check to filter noise
        float speed = (3600000.0 * 1000.0) / (pulseInterval * VSS_PULSES_PER_KM);
        // Cap maximum speed to prevent overflow
        if (speed > 255) speed = 255;
        currentStatus.vss = (uint16_t)speed; // Convert to uint16_t for storage
        //Serial.print("Calculated speed: ");
        //Serial.println(currentStatus.vss);
    }
    
    vssInterruptInProgress = false;
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
    return static_cast<GearSelector>(*gearSelector);
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
        
        // Clean up any existing interrupts on pin 21
        detachInterrupt(digitalPinToInterrupt(21));
        
        // Set up VSS interrupt with hard coded pin 21
        pinMode(21, INPUT);
        //Serial.println("Setting up VSS interrupt on pin 21");
        //Serial.print("Pin 21 state: ");
        //Serial.println(digitalRead(21));
        
        // Try both rising and falling edge
        attachInterrupt(21, vssInterrupt, RISING);
        //Serial.println("VSS interrupt attached on CHANGE edge");
        
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
        
        // 2-3 Shift Curve
        for(byte i = 0; i < 6; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.shift2_3_up_tps[i] = configPage10.flexBoostBins[i];
            configPageTransmission.shift2_3_down_tps[i] = configPage10.flexBoostBins[i];
            
            // VSS points for upshift (using flexBoostAdj)
            configPageTransmission.shift2_3_up_vss[i] = configPage10.flexBoostAdj[i];
            
            // VSS points for downshift (using flexFuelBins)
            configPageTransmission.shift2_3_down_vss[i] = configPage10.flexFuelBins[i];
        }
        
        // 3-4 Shift Curve
        for(byte i = 0; i < 6; i++) {
            // TPS points (using flexBoostBins)
            configPageTransmission.shift3_4_up_tps[i] = configPage10.flexFuelAdj[i];
            configPageTransmission.shift3_4_down_tps[i] = configPage10.flexFuelAdj[i];
            
            // VSS points for upshift (using flexBoostAdj)
            configPageTransmission.shift3_4_up_vss[i] = configPage10.flexAdvBins[i];
            
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
    }
}

void updateTransmission() {

    if (!configPageTransmission.enableTransmission) return;

   
    // Get current engine parameters from CAN inputs
    // Note: currentStatus.vss is now set directly by the VSS interrupt handler
    byte currentTPS = currentStatus.canin[configPageTransmission.canTPSIndex];
    byte currentMAP = currentStatus.canin[configPageTransmission.canMAPIndex];
    byte currentCLT = currentStatus.canin[configPageTransmission.canCLTIndex];
    
    // Update transmission state based on current parameters
    if (getGearSelector() == GearSelector::DRIVE) {
        // Check for upshifts using the 2D tables
        switch (getCurrentGear()) {
            case CurrentGear::FIRST:
                if (currentStatus.vss > table2D_getValue(&shift1_2_up_table, currentTPS)) {
                    // Shift to 2nd
                    setCurrentGear(CurrentGear::SECOND);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::SECOND:
                if (currentStatus.vss > table2D_getValue(&shift2_3_up_table, currentTPS)) {
                    // Shift to 3rd
                    setCurrentGear(CurrentGear::THIRD);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::THIRD:
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
                if (currentStatus.vss < table2D_getValue(&shift1_2_down_table, currentTPS)) {
                    // Shift to 1st
                    setCurrentGear(CurrentGear::FIRST);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::THIRD:
                if (currentStatus.vss < table2D_getValue(&shift2_3_down_table, currentTPS)) {
                    // Shift to 2nd
                    setCurrentGear(CurrentGear::SECOND);
                    lastShiftTime = millis();
                }
                break;
            case CurrentGear::FOURTH:
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