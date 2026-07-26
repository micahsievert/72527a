#include "intake.hpp"

Intake::Intake()
    : intakeMotor(5) {
    
    intakeMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    }

void Intake::setState(IntakeState newState) {
    state = newState;
}

IntakeState Intake::getState() const {
    return state;
}

void Intake::intakeIn() {
    intakeMotor.move(127); // Adjust speed as needed
}

void Intake::intakeOut() {
    intakeMotor.move(-127); // Adjust speed as needed
}

void Intake::intakeOff() {
    intakeMotor.brake();
}

void Intake::update() {
    switch (state) {
        case IntakeState::In:
            intakeIn();
            break;
        case IntakeState::Out:
            intakeOut();
            break;
        case IntakeState::Off:
            intakeOff();
            break;
    }
}