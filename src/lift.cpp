#include "lift.hpp"


Lift::Lift()
    : liftMotor(13) {
    
    liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }

void Lift::initialize() {
    liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    liftMotor.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
    liftMotor.tare_position();

    targetPosition = 0.0;
    state = LiftState::Hold;
}

void Lift::setTarget(double target) {
    targetPosition = target;
    state = LiftState::Automatic;
}
bool Lift::atTarget(double tolerance) {
    double error = targetPosition - getPosition();
    return std::abs(error) <= tolerance;
}
double Lift::getPosition() {
    return liftMotor.get_position();
}
void Lift::updatePositionControl() {
    double error = targetPosition - getPosition();
    double kP = 1.0;
    double output = error * kP;

    double maxPowerUp = 127.0;
    double maxPowerDown = -50.0;

    if (output > maxPowerUp) {
        output = maxPowerUp;
    } else if (output < maxPowerDown) {
        output = maxPowerDown;
    }

    if (std::abs(error) <= 1.0) { // If within 1 degree of target, hold position
        liftHold();
    } else {
        moveLift(output);
    }
}

void Lift::setState(LiftState newState) {
    state = newState;
}

LiftState Lift::getState() const {
    return state;
}

void Lift::moveLift(double power) {
    liftMotor.move(power);
}
void Lift::liftHold() {
    liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    liftMotor.brake();
}

void Lift::update() {
    switch (state) {
        case LiftState::Up:
            moveLift(127);
            break;
        case LiftState::Down:
            moveLift(-70);
            break;
        case LiftState::Hold:
            liftHold();
            break;
        case LiftState::Automatic:
            updatePositionControl();
            break;            
    }
}