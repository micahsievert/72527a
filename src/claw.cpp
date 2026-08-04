#include "claw.hpp"

Claw::Claw(std::uint8_t solenoidPort)
    : solenoid(solenoidPort) {
}

void Claw::setState(ClawState newState) {
    state = newState;
}

ClawState Claw::getState() const {
    return state;
}

void Claw::update() {
    if (state == ClawState::Closed) {
        solenoid.set_value(true);
    } else {
        solenoid.set_value(false);
    }
}