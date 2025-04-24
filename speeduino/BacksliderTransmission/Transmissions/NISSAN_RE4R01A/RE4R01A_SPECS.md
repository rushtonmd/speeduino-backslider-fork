# Nissan RE4R01A Transmission Specifications

## Overview
The RE4R01A is a 4-speed automatic transmission used in various Nissan vehicles. This document outlines the technical specifications and control parameters for the transmission.

## Solenoid Specifications

### Shift Solenoids
1. **Shift Solenoid A (SSA)**
   - Function: Controls 1-2 and 3-4 shifts
   - Normal State: OFF
   - Activation: ON for 1st and 3rd gear

2. **Shift Solenoid B (SSB)**
   - Function: Controls 2-3 shifts
   - Normal State: OFF
   - Activation: ON for 1st and 2nd gear

3. **Shift Solenoid C (SSC)**
   - Function: Controls 3-4 shifts
   - Normal State: OFF
   - Activation: ON for 1st, 2nd, and 3rd gear

### Lockup Solenoid
- **Lockup Control Solenoid (LCS)**
  - Function: Controls torque converter lockup
  - Normal State: OFF
  - Activation: ON for lockup engagement

### Pressure Control Solenoids
1. **Line Pressure Solenoid (LPS)**
   - Function: Controls main line pressure
   - Type: PWM controlled
   - Frequency: 292Hz
   - Duty Cycle Range: 0-100%

2. **Accumulator Control Solenoid (ACS)**
   - Function: Controls shift feel
   - Type: PWM controlled
   - Frequency: 292Hz
   - Duty Cycle Range: 0-100%

## Shift Points
Default shift points (can be adjusted via tuning):

### 1-2 Shift
- Light Throttle: 15-20 km/h
- Medium Throttle: 25-30 km/h
- Heavy Throttle: 35-40 km/h

### 2-3 Shift
- Light Throttle: 30-35 km/h
- Medium Throttle: 45-50 km/h
- Heavy Throttle: 60-65 km/h

### 3-4 Shift
- Light Throttle: 45-50 km/h
- Medium Throttle: 65-70 km/h
- Heavy Throttle: 85-90 km/h

## Lockup Control
- Engagement Speed: 60-65 km/h
- Disengagement Speed: 55-60 km/h
- Minimum TPS for engagement: 15%
- Maximum TPS for disengagement: 85%

## Electrical Specifications

### Connector Pinout
1. **Main Connector (20-pin)**
   - Pin 1: Shift Solenoid A
   - Pin 2: Shift Solenoid B
   - Pin 3: Shift Solenoid C
   - Pin 4: Lockup Control Solenoid
   - Pin 5: Line Pressure Solenoid
   - Pin 6: Accumulator Control Solenoid
   - Pin 7: Ground
   - Pin 8: +12V Power
   - Pin 9: Input Speed Sensor (+)
   - Pin 10: Input Speed Sensor (-)
   - Pin 11: Output Speed Sensor (+)
   - Pin 12: Output Speed Sensor (-)
   - Pin 13: Temperature Sensor
   - Pin 14: Ground
   - Pin 15: Mode Select
   - Pin 16: Inhibitor Switch
   - Pin 17: Ground
   - Pin 18: +12V Power
   - Pin 19: Ground
   - Pin 20: +12V Power

### Sensor Specifications
1. **Input Speed Sensor**
   - Type: Magnetic pickup
   - Pulses per revolution: 20
   - Output: AC voltage

2. **Output Speed Sensor**
   - Type: Magnetic pickup
   - Pulses per revolution: 20
   - Output: AC voltage

3. **Temperature Sensor**
   - Type: NTC thermistor
   - Resistance: 10kΩ at 25°C
   - Temperature Range: -40°C to 150°C

## Fluid Specifications
- Type: Nissan Matic D or equivalent
- Capacity: 7.5L (dry fill)
- Operating Temperature Range: -40°C to 150°C

## Gear Ratios
- 1st: 2.785:1
- 2nd: 1.545:1
- 3rd: 1.000:1
- 4th: 0.694:1
- Reverse: 2.272:1

## Physical Specifications
- Weight: 85kg
- Length: 650mm
- Width: 380mm
- Height: 380mm

## Safety Features
1. **Fail-Safe Mode**
   - Defaults to 3rd gear if electrical system fails
   - Maintains basic functionality without electronic control

2. **Overheat Protection**
   - Disables lockup if fluid temperature exceeds 130°C
   - Reduces line pressure if temperature exceeds 140°C

3. **Pressure Monitoring**
   - Line pressure monitoring for shift quality
   - Pressure relief valve for overpressure protection

## Maintenance Intervals
- Fluid Change: Every 40,000 km
- Filter Change: Every 40,000 km
- Solenoid Cleaning: Every 80,000 km
- Full Rebuild: Every 160,000 km

## Common Issues and Solutions
1. **Harsh Shifts**
   - Check line pressure
   - Verify accumulator control solenoid operation
   - Inspect accumulator springs

2. **Slipping**
   - Check fluid level and condition
   - Verify line pressure
   - Inspect clutch packs

3. **No Lockup**
   - Check lockup solenoid
   - Verify lockup control circuit
   - Inspect torque converter

## Tuning Parameters
1. **Shift Timing**
   - Base timing: 0.5 seconds
   - Maximum timing: 1.0 seconds
   - Minimum timing: 0.2 seconds

2. **Pressure Control**
   - Base pressure: 60 PSI
   - Maximum pressure: 120 PSI
   - Minimum pressure: 30 PSI

3. **Lockup Control**
   - Engagement delay: 0.2 seconds
   - Disengagement delay: 0.1 seconds
   - Minimum engagement time: 1.0 seconds 