#include "lift.hpp"
#include "pros/motors.h"
#include "pros/rotation.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

// Lift controller constants. Tune kD on the robot for the lift's damping.
namespace {
constexpr double kP = 2.5; // Motor power per degree of position error.
constexpr double kD = 0.2; // Motor power per degree/second; large gains can cause oscillation.
constexpr double positionTolerance = 1.5; // Degrees.
constexpr double maxTargetPosition = 1700.0; // Preset limit; manual movement is unrestricted.
}

Lift::Lift(int motorPort, int rotationPort)
    : motor(motorPort), rotationSensor(rotationPort) {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}


void Lift::setState(LiftState newState) {
    state = newState;
}
LiftState Lift::getState() const {
    return state;
}

void Lift::setTarget(double target) {
    if (!std::isfinite(target)) {
        state = LiftState::hold;
        holdMode();
        return;
    }
    targetPosition = std::min(target, maxTargetPosition);
    state = LiftState::preset;
}
double Lift::getTarget() const {
    return targetPosition;
}

bool Lift::atTarget() const {
    return std::abs(getPosition() - targetPosition) <= positionTolerance;
}

double Lift::getPosition() const {
    const auto position = rotationSensor.get_position();
    if (position == PROS_ERR) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return position / 100.0; // Centidegrees to degrees, preserving fractional degrees.
}

void Lift::customUpMode() {
    motor.move(127);
}
void Lift::customDownMode() {
    motor.move(-127);
}
void Lift::holdMode() {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    motor.brake();
}
void Lift::presetMode() {
    const double position = getPosition();
    const auto rawVelocity = rotationSensor.get_velocity();
    if (!std::isfinite(position) || !std::isfinite(targetPosition) || rawVelocity == PROS_ERR) {
        state = LiftState::hold;
        holdMode();
        return;
    }
    const double error = targetPosition - position;
    const double velocity = rawVelocity / 100.0; // Centidegrees/second to degrees/second.

    if (std::abs(error) <= positionTolerance) {
        holdMode();
        return;
    }

    double power = (error * kP) - (velocity * kD);
    if (power > 127) power = 127;
    if (power < -127) power = -127;

    motor.move(power);
}


void Lift::update() {
    switch (state) {
        case LiftState::up:
            customUpMode();
            break;
        case LiftState::down:
            customDownMode();
            break;
        case LiftState::hold:
            holdMode();
            break;
        case LiftState::preset:
            presetMode();
            break;
    }
}
