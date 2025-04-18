# Adding Menu Items and Dialogs in TunerStudio

## Overview
Adding new menu items or dialogs in TunerStudio requires several coordinated steps across different files. This guide outlines the process.

## Step 1: Define the Configuration Structure
1. Add the new configuration variables to `config_pages.h`
   - Define the structure members
   - Add appropriate data types and bit fields
   - Include comments for documentation

## Step 2: Update the INI File
1. Add the new settings to `speeduino.ini`
   - Define the variable in the appropriate section
   - Add descriptive text and units
   - Set default values
   - Add any required conditions or dependencies

2. Add menu/dialog definitions
   - Define the menu structure in the `[Menu]` section
   - Create dialog definitions in `[UiDialogs]` section
   - Add any required panels or fields

## Step 3: Add Power Cycle Requirements (if needed)
If the setting requires a power cycle to take effect:
1. Add `requiresPowerCycle = variableName` to the ini file
2. This will make TunerStudio show a warning when the setting is changed

## Step 4: Build and Test
1. Build the firmware to ensure all changes compile
2. Upload to the device
3. Test the new menu/dialog in TunerStudio
4. Verify settings are saved and loaded correctly

## Example Flow
1. Define in `config_pages.h`:
```c
struct configX {
    byte newSetting : 2;  // Example bit field
    byte anotherSetting;
};
```

2. Add to `speeduino.ini`:
```ini
newSetting = scalar, U08, 123, [0:3], "Setting Name", "Description"
requiresPowerCycle = newSetting

[Menu]
menuItem = newSettings, "New Settings"

[UiDialogs]
dialog = newSettings, "New Settings Dialog"
panel = newSettingsPanel, "Settings Panel"
field = "Setting Name", newSetting
```

## Common Pitfalls
1. Forgetting to add the variable to the config structure
2. Missing default values in the ini file
3. Not adding power cycle requirements when needed
4. Incorrect data types or ranges
5. Missing menu or dialog definitions

## Best Practices
1. Always test changes in a development branch
2. Add clear comments and documentation
3. Use appropriate data types and ranges
4. Consider backward compatibility
5. Test thoroughly before merging to master 