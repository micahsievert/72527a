#include "lift.hpp"


Lift::Lift()
    : liftMotor(6) {
    
    liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }

void Lift::setState(LiftState newState) {
    state = newState;
}

LiftState Lift::getState() const {
    return state;
}

void Lift::liftUp() {
    liftMotor.move(127);
}

void Lift::liftDown() {
    liftMotor.move(-127);
}

void Lift::liftHold() {
    liftMotor.brake();
}

void Lift::update() {
    switch (state) {
        case LiftState::Up:
            liftUp();
            break;
        case LiftState::Down:
            liftDown();
            break;
        case LiftState::Hold:
            liftHold();
            break;            
    }
}