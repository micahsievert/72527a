#include "arm.hpp"

Arm::Arm()
    : armMotor(6) {
    
        armMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }

void Arm::setState(ArmState newState) {
    state = newState;
}

ArmState Arm::getState() const {
    return state;
}

void Arm::armForward() {
    armMotor.move(25);
}

void Arm::armBackward() {
    armMotor.move(-25);
}

void Arm::armHold() {
    armMotor.brake();
}

void Arm::update() {
    switch (state) {
        case(ArmState::Forward):
            armForward();
            break;
        case(ArmState::Backward):
            armBackward();
            break;
        case(ArmState::Hold):
            armHold();
            break;
    }
}