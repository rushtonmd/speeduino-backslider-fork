#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>

// Struct to hold temperature sensor parameters
struct TempSensor {
    uint8_t pin;        // Analog pin number
    float r1;          // Pull-up resistor value in ohms
    float bValue;      // B-parameter value
    float t0;          // Reference temperature in Kelvin
    float r0;          // Resistance at reference temperature
    float lastTemp;    // Last calculated temperature
};

// Function declarations
void initTempSensor(TempSensor* sensor, uint8_t pin, float pullupResistor, 
                   float t1, float r1, float t2, float r2);  // t1,r1 and t2,r2 are temperature-resistance pairs
void beginTempSensor(TempSensor* sensor);
uint16_t readRawTemp(TempSensor* sensor);
float adcToResistance(TempSensor* sensor, uint16_t adcValue);
float calculateTemperature(TempSensor* sensor);
float getTemperature(TempSensor* sensor);
void setCalibration(TempSensor* sensor, float r1, float bValue, float t0, float r0);

#endif // TEMPERATURE_SENSOR_H 