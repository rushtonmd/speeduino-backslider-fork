#include "PaddleShifter.h"
#include "BacksliderTransmission.h"

/*
 * Paddle Shifter Logic
 * --------------------
 * This module handles paddle shifter functionality for a transmission control system.
 * 
 * Hardware Configuration:
 * - Paddle 1 (configPage16.paddle_shifter_pin_1): Upshift paddle
 * - Paddle 2 (configPage16.paddle_shifter_pin_2): Downshift paddle
 * - Both paddles use active-low logic with internal pullup resistors
 * - Ground triggers: Paddle pressed = LOW, Paddle released = HIGH
 * 
 * Shift Modes:
 * - PADDLE_SHIFT_MODE_AUTO (0): Paddle shifts are optional, transmission operates automatically
 * - PADDLE_SHIFT_MODE_MANUAL (1): Manual mode, transmission only shifts via paddle input
 * 
 * IMPORTANT: Both AUTO and MANUAL modes are ONLY available when gear selector is in DRIVE
 * 
 * Operation:
 * 1. Single Paddle Press (Only works in DRIVE):
 *    - Paddle 1 (upshift): Shifts up one gear (1->2, 2->3, 3->4)
 *    - Paddle 2 (downshift): Shifts down one gear (4->3, 3->2, 2->1)
 *    - In AUTO mode: Paddle shifts override automatic shifting temporarily
 *    - In MANUAL mode: Only paddle shifts control gear changes
 * 
 * 2. Both Paddles Pressed (Only works in DRIVE):
 *    - If held for PADDLE_SHIFTER_MODE_TOGGLE_DELAY milliseconds:
 *      Toggles between AUTO and MANUAL shift modes
 * 
 * 3. Other Gear Selector Positions:
 *    - PARK, REVERSE, NEUTRAL, 1st, 2nd, 3rd: Paddle shifters have NO effect
 *    - Transmission is locked to the selected gear/mode
 *    - Shift mode status remains unchanged but is inactive
 * 
 * 4. Debouncing:
 *    - Uses PADDLE_SHIFTER_DEBOUNCE_TIME to prevent multiple/spurious shifts
 *    - Ensures minimum time between shifts to avoid mechanical issues
 *    - Prevents rapid up-down shift scenarios
 * 
 * 5. State Recovery:
 *    - Periodically checks actual pin states to recover from missed interrupts
 *    - Automatically corrects paddle states if they become out of sync
 *    - Clears stuck shift requests after a timeout
 * 
 * Status Variables:
 * - currentStatus.paddleShifter1_State: True when paddle 1 is pressed
 * - currentStatus.paddleShifter2_State: True when paddle 2 is pressed
 * - currentStatus.paddleShifter_ShiftMode: Current shift mode (AUTO/MANUAL)
 * 
 * Interrupt Handling:
 * - Uses CHANGE interrupts to detect both press and release events
 * - ISRs update paddle states and request shift actions
 * - Main loop processes shift requests and mode changes ONLY in DRIVE
 */

// Global variables
volatile bool paddleShiftUpRequested = false;
volatile bool paddleShiftDownRequested = false;
volatile uint32_t lastPaddleShiftTime = 0;
uint32_t lastShiftTime = 0;  // Time of last gear shift
static uint32_t bothPaddlesPressedTime = 0;
static bool waitingForModeToggle = false;
static uint32_t lastStateCheckTime = 0;
static const uint32_t STATE_CHECK_INTERVAL = 50; // Check actual pin states every 50ms
static uint32_t paddleUpRequestTime = 0;
static uint32_t paddleDownRequestTime = 0;
static const uint32_t SHIFT_REQUEST_TIMEOUT = 500; // Clear stuck requests after 500ms

// Interrupt Service Routines for paddle press (CHANGE edge)
void paddleShifterUpISR() {
    uint32_t currentTime = millis();
    uint32_t timeSinceLastShift = currentTime - lastShiftTime;
    bool pinState = digitalRead(configPage16.paddle_shifter_pin_1);  // Paddle 1 for upshift
    
    if (timeSinceLastShift >= PADDLE_SHIFTER_DEBOUNCE_TIME) {
        if (pinState == LOW) {  // Pressed (active low with pullup)
            paddleShiftUpRequested = true;
            paddleUpRequestTime = currentTime;
            currentStatus.paddleShifter1_State = true;
        } else {  // Released
            currentStatus.paddleShifter1_State = false;
            // Don't clear paddleShiftUpRequested here - let updatePaddleShifters() handle it
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
            paddleDownRequestTime = currentTime;
            currentStatus.paddleShifter2_State = true;
        } else {  // Released
            currentStatus.paddleShifter2_State = false;
            // Don't clear paddleShiftDownRequested here - let updatePaddleShifters() handle it
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
    
    // Initialize states based on current pin readings
    currentStatus.paddleShifter1_State = (digitalRead(configPage16.paddle_shifter_pin_1) == LOW);
    currentStatus.paddleShifter2_State = (digitalRead(configPage16.paddle_shifter_pin_2) == LOW);
}

// Function to check and correct paddle states
void checkPaddleStates() {
    uint32_t currentTime = millis();
    
    // Read actual pin states (LOW = pressed, HIGH = released)
    bool actualState1 = (digitalRead(configPage16.paddle_shifter_pin_1) == LOW);
    bool actualState2 = (digitalRead(configPage16.paddle_shifter_pin_2) == LOW);
    
    // Force correct state if it doesn't match actual pin reading
    currentStatus.paddleShifter1_State = actualState1;
    currentStatus.paddleShifter2_State = actualState2;
    
    // Clear stuck shift requests if paddle is actually released
    if (!actualState1 && paddleShiftUpRequested && 
        (currentTime - paddleUpRequestTime) > SHIFT_REQUEST_TIMEOUT) {
        paddleShiftUpRequested = false;
    }
    
    if (!actualState2 && paddleShiftDownRequested && 
        (currentTime - paddleDownRequestTime) > SHIFT_REQUEST_TIMEOUT) {
        paddleShiftDownRequested = false;
    }
}

void updatePaddleShifters() {
    uint32_t currentTime = millis();
    
    // Periodically check actual pin states to recover from missed interrupts
    if ((currentTime - lastStateCheckTime) >= STATE_CHECK_INTERVAL) {
        checkPaddleStates();
        lastStateCheckTime = currentTime;
    }
    
    // Only process paddle shifter inputs when gear selector is in DRIVE
    if (getGearSelector() != GearSelector::DRIVE) {
        // Clear any pending shift requests when not in DRIVE
        paddleShiftUpRequested = false;
        paddleShiftDownRequested = false;
        waitingForModeToggle = false;
        return;
    }
    
    // Check paddle shifter states (only executed when in DRIVE)
    if (paddleShiftUpRequested && !paddleShiftDownRequested) {
        // Paddle 1 pressed - shift up
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
        paddleShiftUpRequested = false;
    }
    else if (paddleShiftDownRequested && !paddleShiftUpRequested) {
        // Paddle 2 pressed - shift down
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