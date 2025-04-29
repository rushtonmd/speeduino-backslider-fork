#include "TemperatureSensor.h"

void initTempSensor(TempSensor* sensor, uint8_t pin, float pullupResistor, 
                   float t1, float r1, float t2, float r2) {
    sensor->pin = pin;
    sensor->r1 = pullupResistor;
    
    // Convert temperatures to Kelvin
    float t1K = t1 + 273.15;
    float t2K = t2 + 273.15;
    
    // Calculate B-value using the two-point method
    // B = ln(R1/R2) / (1/T1 - 1/T2)
    sensor->bValue = log(r1/r2) / ((1.0/t1K) - (1.0/t2K));
    
    // Set reference temperature and resistance (using the first point)
    sensor->t0 = t1K;
    sensor->r0 = r1;
    
    sensor->lastTemp = 0.0;
}

void beginTempSensor(TempSensor* sensor) {
    pinMode(sensor->pin, INPUT);
}

uint16_t readRawTemp(TempSensor* sensor) {
    return analogRead(sensor->pin);
}

float adcToResistance(TempSensor* sensor, uint16_t adcValue) {
    // Convert ADC reading to voltage
    float voltage = (adcValue * 3.3) / 4095.0;  // Assuming 3.3V reference and 12-bit ADC
    
    // Calculate thermistor resistance using voltage divider equation
    // R = R1 * (Vcc - V) / V
    float resistance = sensor->r1 * (3.3 - voltage) / voltage;
    
    return resistance;
}

float calculateTemperature(TempSensor* sensor) {
    uint16_t adcValue = readRawTemp(sensor);
    float resistance = adcToResistance(sensor, adcValue);
    
    // B-parameter equation: 1/T = 1/T0 + (1/B) * ln(R/R0)
    // T = 1 / (1/T0 + (1/B) * ln(R/R0))
    float t = 1.0 / ((1.0 / sensor->t0) + (1.0 / sensor->bValue) * log(resistance / sensor->r0));
    
    // Convert from Kelvin to Celsius
    sensor->lastTemp = t - 273.15;
    
    return sensor->lastTemp;
}

float getTemperature(TempSensor* sensor) {
    return sensor->lastTemp;
}

void setCalibration(TempSensor* sensor, float r1, float bValue, float t0, float r0) {
    sensor->r1 = r1;
    sensor->bValue = bValue;
    sensor->t0 = t0;
    sensor->r0 = r0;
} 