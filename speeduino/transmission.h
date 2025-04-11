#pragma once
#include <Arduino.h>

// Transmission Settings
extern uint8_t transEnabled;
extern uint8_t transType;
extern uint8_t transControlType;
extern uint8_t transGearCount;
extern uint8_t transMinRPM;
extern uint8_t transMaxRPM;
extern uint8_t transRPMHyst;
extern uint8_t transMinTPS;
extern uint8_t transMaxTPS;
extern uint8_t transTPSHyst;

// Transmission Input Settings
extern uint8_t transInputPin1;
extern uint8_t transInputPol1;
extern uint8_t transInputEnabled1;
extern uint8_t transInputPin2;
extern uint8_t transInputPol2;
extern uint8_t transInputEnabled2;
extern uint8_t transInputPin3;
extern uint8_t transInputPol3;
extern uint8_t transInputEnabled3;
extern uint8_t transInputPin4;
extern uint8_t transInputPol4;
extern uint8_t transInputEnabled4;

// Transmission Output Settings
extern uint8_t transOutputPin1;
extern uint8_t transOutputPol1;
extern uint8_t transOutputEnabled1;
extern uint8_t transOutputPin2;
extern uint8_t transOutputPol2;
extern uint8_t transOutputEnabled2;
extern uint8_t transOutputPin3;
extern uint8_t transOutputPol3;
extern uint8_t transOutputEnabled3;
extern uint8_t transOutputPin4;
extern uint8_t transOutputPol4;
extern uint8_t transOutputEnabled4;

// Paddle Shifter Settings
extern uint8_t paddleUpPin;
extern uint8_t paddleUpPol;
extern uint8_t paddleUpEnabled;
extern uint8_t paddleDownPin;
extern uint8_t paddleDownPol;
extern uint8_t paddleDownEnabled;
extern uint8_t paddleShiftDelay;
extern uint8_t paddleMinRPM; 