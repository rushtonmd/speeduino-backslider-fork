# Setting Up Transmission Control Menu Structure

## Overview
This guide details how to set up a complete transmission control menu structure in TunerStudio, including main menu and submenus with their respective dialogs.

## Step 1: Define Transmission Configuration Structure
In `config_pages.h`, add a new structure for transmission settings:
```c
struct configTransmission {
    // General Settings
    byte transEnabled : 1;          // Enable/disable transmission control
    byte transType : 2;             // Transmission type (0=Manual, 1=Automatic, 2=Other)
    byte unused1 : 5;               // Reserved bits

    // Shift Settings
    byte upshiftRPM;                // RPM for upshift
    byte downshiftRPM;              // RPM for downshift
    byte shiftDelay;                // Delay between shifts (ms)
    
    // Pressure Control
    byte linePressure;              // Line pressure setting
    byte pressureModifier;          // Pressure modifier based on conditions
    
    // Temperature Protection
    byte maxTemp;                   // Maximum transmission temperature
    byte tempWarning;               // Temperature warning threshold
    
    // Gear Ratios
    uint16_t gearRatio[6];          // Gear ratios (multiplied by 1000)
    
    // Additional settings as needed...
};
```

## Step 2: Update Page System Configuration
In `speeduino.ini`, update the following sections:

### Update nPages and pageSize
```ini
[Constants]
nPages = 16                        ; Increase from 15 to 16 to add transmission page
pageSize = 128, 288, 288, 128, 288, 128, 240, 384, 192, 192, 288, 192, 128, 288, 256, 32
```

### Add Page Commands
```ini
pageIdentifier = "\$tsCanId\x01", "\$tsCanId\x02", "\$tsCanId\x03", "\$tsCanId\x04", "\$tsCanId\x05", "\$tsCanId\x06", "\$tsCanId\x07", "\$tsCanId\x08", "\$tsCanId\x09", "\$tsCanId\x0A", "\$tsCanId\x0B", "\$tsCanId\x0C", "\$tsCanId\x0D", "\$tsCanId\x0E", "\$tsCanId\x0F", "\$tsCanId\x10"
pageReadCommand = "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c", "p%2i%2o%2c"
pageValueWrite = "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v"
pageChunkWrite = "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v", "M%2i%2o%2c%v"
crc32CheckCommand = "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i", "d%2i"
#if COMMS_COMPAT
    burnCommand = "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i", "B%2i"
#else
    burnCommand = "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i", "b%2i"
#endif
```

### Define Transmission Page Constants
```ini
[Constants]
; Transmission Settings (Page 16)
transEnabled = scalar, U08, 0, "Enable", "Transmission Control", 0, 1, 1, 0
transType = scalar, U08, 1, "Type", "Transmission Type", 0, 2, 1, 0
upshiftRPM = scalar, U08, 2, "RPM", "Upshift RPM", 0, 255, 1, 0
downshiftRPM = scalar, U08, 3, "RPM", "Downshift RPM", 0, 255, 1, 0
shiftDelay = scalar, U08, 4, "ms", "Shift Delay", 0, 255, 1, 0
linePressure = scalar, U08, 5, "%", "Line Pressure", 0, 100, 1, 0
pressureModifier = scalar, U08, 6, "%", "Pressure Modifier", 0, 100, 1, 0
maxTemp = scalar, U08, 7, "°C", "Max Temperature", 0, 255, 1, 0
tempWarning = scalar, U08, 8, "°C", "Warning Temperature", 0, 255, 1, 0
gearRatio1 = scalar, U16, 9, "x1000", "1st Gear Ratio", 0, 65535, 1, 0
gearRatio2 = scalar, U16, 11, "x1000", "2nd Gear Ratio", 0, 65535, 1, 0
gearRatio3 = scalar, U16, 13, "x1000", "3rd Gear Ratio", 0, 65535, 1, 0
gearRatio4 = scalar, U16, 15, "x1000", "4th Gear Ratio", 0, 65535, 1, 0
gearRatio5 = scalar, U16, 17, "x1000", "5th Gear Ratio", 0, 65535, 1, 0
gearRatio6 = scalar, U16, 19, "x1000", "6th Gear Ratio", 0, 65535, 1, 0
```

## Step 3: Update Menu Structure

### Main Menu Definition
```ini
[Menu]
menuItem = transmission, "TRANSMISSION"
```

### Submenu Definitions
```ini
[Menu]
subMenu = transmission_general, "General Settings", transmission
subMenu = transmission_shifting, "Shift Settings", transmission
subMenu = transmission_pressure, "Pressure Control", transmission
subMenu = transmission_protection, "Temperature Protection", transmission
subMenu = transmission_ratios, "Gear Ratios", transmission
```

### Dialog Definitions
```ini
[UiDialogs]
; General Settings Dialog
dialog = transmission_general, "Transmission General Settings"
panel = transGeneralPanel, "General Settings"
field = "Transmission Control", transEnabled, {transEnabled == 1 || transEnabled == 0}
field = "Transmission Type", transType, {transType >= 0 && transType <= 2}

; Shift Settings Dialog
dialog = transmission_shifting, "Transmission Shift Settings"
panel = transShiftPanel, "Shift Settings"
field = "Upshift RPM", upshiftRPM, {upshiftRPM > 0}
field = "Downshift RPM", downshiftRPM, {downshiftRPM > 0}
field = "Shift Delay (ms)", shiftDelay, {shiftDelay >= 0}

; Pressure Control Dialog
dialog = transmission_pressure, "Transmission Pressure Control"
panel = transPressurePanel, "Pressure Settings"
field = "Line Pressure", linePressure, {linePressure >= 0 && linePressure <= 100}
field = "Pressure Modifier", pressureModifier, {pressureModifier >= 0 && pressureModifier <= 100}

; Temperature Protection Dialog
dialog = transmission_protection, "Transmission Temperature Protection"
panel = transTempPanel, "Temperature Settings"
field = "Max Temperature (°C)", maxTemp, {maxTemp > 0}
field = "Warning Temperature (°C)", tempWarning, {tempWarning > 0}

; Gear Ratios Dialog
dialog = transmission_ratios, "Transmission Gear Ratios"
panel = transRatioPanel, "Gear Ratio Settings"
field = "1st Gear Ratio", gearRatio1, {gearRatio1 > 0}
field = "2nd Gear Ratio", gearRatio2, {gearRatio2 > 0}
field = "3rd Gear Ratio", gearRatio3, {gearRatio3 > 0}
field = "4th Gear Ratio", gearRatio4, {gearRatio4 > 0}
field = "5th Gear Ratio", gearRatio5, {gearRatio5 > 0}
field = "6th Gear Ratio", gearRatio6, {gearRatio6 > 0}
```

## Step 4: Add Power Cycle Requirements
```ini
requiresPowerCycle = transEnabled
requiresPowerCycle = transType
```

## Step 5: Implementation Tips

### Page System Notes
1. Page size should be large enough to hold all variables (32 bytes in this case)
2. Each page needs corresponding commands for read/write operations
3. All command arrays must match the number of pages
4. Memory offsets are handled automatically by the page system

### Menu Organization
1. Group related settings in logical submenus
2. Use clear, descriptive names for menus and fields
3. Add appropriate units to field descriptions
4. Include validation ranges where applicable

### Data Types
1. Use appropriate data types for each setting:
   - `byte` for simple on/off or small range values
   - `uint16_t` for larger values like gear ratios
   - Bit fields for multiple related boolean settings

### Validation
1. Add range checks for all numeric values
2. Include dependencies between settings
3. Add warning conditions for critical values

### Testing
1. Test each dialog independently
2. Verify all settings save and load correctly
3. Test power cycle requirements
4. Verify menu navigation works as expected

## Example Usage Flow
1. User selects "TRANSMISSION" from main menu
2. Submenu options appear:
   - General Settings
   - Shift Settings
   - Pressure Control
   - Temperature Protection
   - Gear Ratios
3. Each submenu opens its respective dialog
4. Changes require power cycle where specified

## Common Considerations
1. Memory usage in the config structure
2. EEPROM storage requirements
3. Backward compatibility
4. Default values for new installations
5. Documentation for end users 