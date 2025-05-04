#ifndef PADDLE_SHIFTER_H
#define PADDLE_SHIFTER_H

#include "globals.h"
#include "pages.h"
#include "BacksliderTransmission.h"

// Constants
#define PADDLE_SHIFTER_DEBOUNCE_TIME 200  // Debounce time in milliseconds
#define PADDLE_SHIFTER_MODE_TOGGLE_DELAY 2000  // Mode toggle delay in milliseconds

// Shift modes
#define PADDLE_SHIFT_MODE_AUTO 0   // Automatic mode - transmission shifts automatically
#define PADDLE_SHIFT_MODE_MANUAL 1 // Manual mode - transmission only shifts with paddle input

// Function declarations
void initPaddleShifters();
void paddleShifterUpISR();
void paddleShifterDownISR();
void paddleShifterUpReleaseISR();
void paddleShifterDownReleaseISR();
void updatePaddleShifters();

// Inline function definitions
inline GearSelector getGearSelector() {
    return static_cast<GearSelector>(currentStatus.gearSelectorChosenGear);
}

inline CurrentGear getCurrentGear() {
    return static_cast<CurrentGear>(currentStatus.gear);
}

inline void setCurrentGear(CurrentGear gear) {
    currentStatus.gear = static_cast<uint8_t>(gear);
}

// Variables
extern volatile bool paddleShiftUpRequested;
extern volatile bool paddleShiftDownRequested;
extern volatile uint32_t lastPaddleShiftTime;
extern uint32_t lastShiftTime;  // Time of last gear shift

#endif // PADDLE_SHIFTER_H 