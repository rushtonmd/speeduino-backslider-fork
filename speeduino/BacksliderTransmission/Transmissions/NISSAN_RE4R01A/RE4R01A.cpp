#include "RE4R01A.h"

// Define the solenoid state arrays
const SolenoidState SolenoidStates::FIRST_GEAR[3] = {SolenoidState::ON, SolenoidState::ON, SolenoidState::ON};
const SolenoidState SolenoidStates::SECOND_GEAR[3] = {SolenoidState::OFF, SolenoidState::ON, SolenoidState::ON};
const SolenoidState SolenoidStates::THIRD_GEAR[3] = {SolenoidState::ON, SolenoidState::OFF, SolenoidState::ON};
const SolenoidState SolenoidStates::FOURTH_GEAR[3] = {SolenoidState::OFF, SolenoidState::OFF, SolenoidState::OFF}; 