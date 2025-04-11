#include "transmission.h"

// Transmission Settings
uint8_t transEnabled = 0;
uint8_t transType = 0;
uint8_t transControlType = 0;
uint8_t transGearCount = 0;
uint8_t transMinRPM = 0;
uint8_t transMaxRPM = 0;
uint8_t transRPMHyst = 0;
uint8_t transMinTPS = 0;
uint8_t transMaxTPS = 0;
uint8_t transTPSHyst = 0;

// Transmission Input Settings
uint8_t transInputPin1 = 0;
uint8_t transInputPol1 = 0;
uint8_t transInputEnabled1 = 0;
uint8_t transInputPin2 = 0;
uint8_t transInputPol2 = 0;
uint8_t transInputEnabled2 = 0;
uint8_t transInputPin3 = 0;
uint8_t transInputPol3 = 0;
uint8_t transInputEnabled3 = 0;
uint8_t transInputPin4 = 0;
uint8_t transInputPol4 = 0;
uint8_t transInputEnabled4 = 0;

// Transmission Output Settings
uint8_t transOutputPin1 = 0;
uint8_t transOutputPol1 = 0;
uint8_t transOutputEnabled1 = 0;
uint8_t transOutputPin2 = 0;
uint8_t transOutputPol2 = 0;
uint8_t transOutputEnabled2 = 0;
uint8_t transOutputPin3 = 0;
uint8_t transOutputPol3 = 0;
uint8_t transOutputEnabled3 = 0;
uint8_t transOutputPin4 = 0;
uint8_t transOutputPol4 = 0;
uint8_t transOutputEnabled4 = 0;

// Paddle Shifter Settings
uint8_t paddleUpPin = 0;
uint8_t paddleUpPol = 0;
uint8_t paddleUpEnabled = 0;
uint8_t paddleDownPin = 0;
uint8_t paddleDownPol = 0;
uint8_t paddleDownEnabled = 0;
uint8_t paddleShiftDelay = 0;
uint8_t paddleMinRPM = 0; 