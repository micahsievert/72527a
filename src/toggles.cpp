#include "toggles.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace {
constexpr int toggleVelocity = 1000; // RPM.
constexpr double toggleDegrees = 360.0 * 1.5; // compensate for 12:18 gear ratio
constexpr double positionTolerance = 3.0; // Motor degrees.
}

Toggles::Toggles(int leftMotor, int rightMotor)
    : leftMotor(leftMotor), rightMotor(rightMotor) {
}

void Toggles::initialize() {
    leftMotor.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
    rightMotor.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
    leftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    rightMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void Toggles::toggleOnce() {
    if (!targetsInitialized) {
        const double leftPosition = leftMotor.get_position();
        const double rightPosition = rightMotor.get_position();
        if (!std::isfinite(leftPosition) || !std::isfinite(rightPosition)) return;
        leftTarget = leftPosition;
        rightTarget = rightPosition;
        targetsInitialized = true;
    }

    // Advance exact cycle targets rather than accumulating stopping error.
    leftTarget += toggleDegrees;
    rightTarget += toggleDegrees;
    leftMotor.move_absolute(leftTarget, toggleVelocity);
    rightMotor.move_absolute(rightTarget, toggleVelocity);
    cycleRunning = true;
}

void Toggles::waitUntilDone() {
    while (cycleRunning) {
        update(false);
        if (cycleRunning) pros::delay(10);
    }
}

void Toggles::update(bool pressed) {
    if (cycleRunning) {
        const double leftPosition = leftMotor.get_position();
        const double rightPosition = rightMotor.get_position();
        if (std::isfinite(leftPosition) && std::isfinite(rightPosition) &&
            std::abs(leftTarget - leftPosition) <= positionTolerance &&
            std::abs(rightTarget - rightPosition) <= positionTolerance) {
            cycleRunning = false;
        }
    }

    // Queue no extra cycles: release lets the current position move finish.
    if (pressed && !cycleRunning) {
        toggleOnce();
    }
}
