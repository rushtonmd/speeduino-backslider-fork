# Speeduino Notes

## Core Functions

### initialiseAll
Location: `speeduino/init.cpp`

This is the main initialization function called during `setup()`. It's responsible for setting up all hardware and software components of Speeduino.

#### Main Sections:

1. **Pin and Hardware Setup**
   - Sets up all I/O pins
   - Configures timers and interrupts
   - Initializes communication interfaces (Serial, CAN)

2. **Memory and Storage**
   - EEPROM initialization
   - Loads configuration from storage
   - Sets up default values

3. **Sensor Setup**
   - Calibrates analog inputs
   - Sets up temperature sensors
   - Configures pressure sensors

4. **Engine Control Setup**
   - Fuel system initialization
   - Ignition system setup
   - Idle control configuration

5. **Communication Setup**
   - Serial protocol initialization
   - CAN bus setup
   - Secondary serial interface

6. **Safety Systems**
   - Engine protection setup
   - Fault detection initialization
   - System monitoring

#### Usage:
```cpp
void setup(void)
{
  currentStatus.initialisationComplete = false;
  initialiseAll();
}
``` 