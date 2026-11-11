#include "pivot.hpp"
#include "pros/motors.h"
#include "pros/rotation.hpp"
#include <cmath>

// Pivot Controller Constants
constexpr double kP = 0.8; // Motor power per degree of position error.
constexpr double kD = 1; // Motor power per degree/second of measured velocity.
constexpr double positionTolerance = 3.0; // Degrees.

Pivot::Pivot(int motorPort, int rotationPort)
    : motor(motorPort), rotationSensor(rotationPort) {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}


void Pivot::setState(PivotState newState) {
    state = newState;
}
PivotState Pivot::getState() const {
    return state;
}

void Pivot::setTarget(double target) {
    targetPosition = target;
    state = PivotState::preset;
}
double Pivot::getTarget() const {
    return targetPosition;
}

bool Pivot::atTarget() const{
    return std::abs(getPosition() - targetPosition) <= positionTolerance;
}

double Pivot::getPosition() const {
    return rotationSensor.get_position() / 100.0; // Centidegrees to degrees.
}

void Pivot::customUpMode() {
    motor.move(70);
}
void Pivot::customDownMode() {
    motor.move(-50);
}
void Pivot::holdMode() {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    motor.brake();
}
void Pivot::presetMode() {
    double error = targetPosition - getPosition();
    double velocity = rotationSensor.get_velocity() / 100.0;

    if (std::abs(error) <= positionTolerance) {
        holdMode();
        return;
    }
    
    double power = (error * kP) - (velocity * kD);

    if (power > 127) power = 127;
    if (power < -127) power = -127;

    motor.move(power);
}


void Pivot::update() {
    switch (state) {
        case PivotState::up:
            customUpMode();
            break;
        case PivotState::down:
            customDownMode();
            break;
        case PivotState::hold:
            holdMode();
            break;
        case PivotState::preset:
            presetMode();
            break;
    }
}
