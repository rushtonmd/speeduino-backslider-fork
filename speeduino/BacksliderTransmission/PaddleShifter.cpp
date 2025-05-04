#include "PaddleShifter.h"
#include "BacksliderTransmission.h"

// Global variables
volatile bool paddleShiftUpRequested = false;
volatile bool paddleShiftDownRequested = false;
volatile uint32_t lastPaddleShiftTime = 0;
uint32_t lastShiftTime = 0;  // Time of last gear shift
static uint32_t bothPaddlesPressedTime = 0;
static bool waitingForModeToggle = false;

// Interrupt Service Routines for paddle press (CHANGE edge)
void paddleShifterUpISR() {
    uint32_t currentTime = millis();
    uint32_t timeSinceLastShift = currentTime - lastShiftTime;
    bool pinState = digitalRead(configPage16.paddle_shifter_pin_1);  // Paddle 1 for upshift
    
    if (timeSinceLastShift >= PADDLE_SHIFTER_DEBOUNCE_TIME) {
        if (pinState == LOW) {  // Pressed (active low with pullup)
            paddleShiftUpRequested = true;
            currentStatus.paddleShifter1_State = true;
        } else {  // Released
            currentStatus.paddleShifter1_State = false;
        }
    }
}

void paddleShifterDownISR() {
    uint32_t currentTime = millis();
    uint32_t timeSinceLastShift = currentTime - lastShiftTime;
    bool pinState = digitalRead(configPage16.paddle_shifter_pin_2);  // Paddle 2 for downshift
    
    if (timeSinceLastShift >= PADDLE_SHIFTER_DEBOUNCE_TIME) {
        if (pinState == LOW) {  // Pressed (active low with pullup)
            paddleShiftDownRequested = true;
            currentStatus.paddleShifter2_State = true;
        } else {  // Released
            currentStatus.paddleShifter2_State = false;
        }
    }
}

void initPaddleShifters() {
    // Configure paddle shifter pins as inputs with pullup resistors
    pinMode(configPage16.paddle_shifter_pin_1, INPUT_PULLUP);
    pinMode(configPage16.paddle_shifter_pin_2, INPUT_PULLUP);
    
    // Attach interrupts for both pins using CHANGE mode
    // This will trigger on both press and release
    attachInterrupt(digitalPinToInterrupt(configPage16.paddle_shifter_pin_1), paddleShifterUpISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(configPage16.paddle_shifter_pin_2), paddleShifterDownISR, CHANGE);
}

void updatePaddleShifters() {
    // Check paddle shifter states
    if (paddleShiftUpRequested && !paddleShiftDownRequested) {
        // Paddle 1 pressed - shift up
        if (currentStatus.paddleShifter_ShiftMode == PADDLE_SHIFT_MODE_MANUAL || getGearSelector() == GearSelector::DRIVE) {
            switch (getCurrentGear()) {
                case CurrentGear::FIRST:
                    setCurrentGear(CurrentGear::SECOND);
                    break;
                case CurrentGear::SECOND:
                    setCurrentGear(CurrentGear::THIRD);
                    break;
                case CurrentGear::THIRD:
                    setCurrentGear(CurrentGear::FOURTH);
                    break;
                default:
                    break;
            }
            lastShiftTime = millis();
        }
        paddleShiftUpRequested = false;
    }
    else if (paddleShiftDownRequested && !paddleShiftUpRequested) {
        // Paddle 2 pressed - shift down
        if (currentStatus.paddleShifter_ShiftMode == PADDLE_SHIFT_MODE_MANUAL || getGearSelector() == GearSelector::DRIVE) {
            switch (getCurrentGear()) {
                case CurrentGear::SECOND:
                    setCurrentGear(CurrentGear::FIRST);
                    break;
                case CurrentGear::THIRD:
                    setCurrentGear(CurrentGear::SECOND);
                    break;
                case CurrentGear::FOURTH:
                    setCurrentGear(CurrentGear::THIRD);
                    break;
                default:
                    break;
            }
            lastShiftTime = millis();
        }
        paddleShiftDownRequested = false;
    }
    else if (paddleShiftUpRequested && paddleShiftDownRequested) {
        // Both paddles pressed - handle mode toggle
        if (!waitingForModeToggle) {
            bothPaddlesPressedTime = millis();
            waitingForModeToggle = true;
        }
        else if ((millis() - bothPaddlesPressedTime) >= PADDLE_SHIFTER_MODE_TOGGLE_DELAY) {
            // Toggle between auto and manual mode
            if (currentStatus.paddleShifter_ShiftMode == PADDLE_SHIFT_MODE_AUTO) {
                currentStatus.paddleShifter_ShiftMode = PADDLE_SHIFT_MODE_MANUAL;
            }
            else {
                currentStatus.paddleShifter_ShiftMode = PADDLE_SHIFT_MODE_AUTO;
            }
            waitingForModeToggle = false;
            paddleShiftUpRequested = false;
            paddleShiftDownRequested = false;
        }
    }
    else {
        // Reset mode toggle if paddles are released
        waitingForModeToggle = false;
    }
}