#include "claw.hpp"
#include "drivetrain.hpp"
#include "pros/motors.h"
#include "main.hpp"

Claw::Claw(int motorPort)
    : motor(motorPort) {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void Claw::setState(ClawState newState) {
    state = newState;
}
ClawState Claw::getState() const {
    return state;
}


void Claw::grabbingMode() {
    motor.move(127);
}
void Claw::releasingMode() {
    motor.move(-63);
}
void Claw::offMode() {
    motor.move(0);
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}


void Claw::update() {
    switch (state) {
        case ClawState::grabbing:
            grabbingMode();
            break;
        case ClawState::releasing:
            releasingMode();
            break;
        case ClawState::off:
            offMode();
            break;
    }
}