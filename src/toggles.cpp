#include "toggles.hpp"
#include "pros/rtos.hpp"
#include <cmath>

namespace {
constexpr int toggleVelocity = 1000; // RPM.
constexpr double toggleDegrees = 360.0;
constexpr double positionTolerance = 3.0;  // Motor degrees.
constexpr double maxDrift = 45.0;          // If a motor is this far off its target, start fresh from where it is
constexpr std::uint32_t cycleTimeoutMs = 1000; // Give up on a cycle after this long
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
    const double leftPosition = leftMotor.get_position();
    const double rightPosition = rightMotor.get_position();
    if (!std::isfinite(leftPosition) || !std::isfinite(rightPosition)) return; // a motor isn't responding

    // First cycle, or a motor drifted / reset its position: start fresh from where it actually is.
    // (Without this, a motor that briefly loses power resets to 0 and spins many turns to catch up.)
    if (!targetsInitialized || std::abs(leftTarget - leftPosition) > maxDrift)   leftTarget = leftPosition;
    if (!targetsInitialized || std::abs(rightTarget - rightPosition) > maxDrift) rightTarget = rightPosition;
    targetsInitialized = true;

    leftTarget += toggleDegrees;
    rightTarget += toggleDegrees;
    leftMotor.move_absolute(leftTarget, toggleVelocity);
    rightMotor.move_absolute(rightTarget, toggleVelocity);
    cycleRunning = true;
    cycleStartMs = pros::millis();
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
        const bool readingsOk = std::isfinite(leftPosition) && std::isfinite(rightPosition);

        // A motor suddenly way off target means it reset mid-cycle: stop it instead of letting it run away
        const bool motorReset = readingsOk &&
            (std::abs(leftTarget - leftPosition) > toggleDegrees + maxDrift ||
             std::abs(rightTarget - rightPosition) > toggleDegrees + maxDrift);

        const bool done = readingsOk &&
            std::abs(leftTarget - leftPosition) <= positionTolerance &&
            std::abs(rightTarget - rightPosition) <= positionTolerance;

        const bool timedOut = pros::millis() - cycleStartMs > cycleTimeoutMs;

        if (motorReset || timedOut) {
            leftMotor.move_velocity(0);   // stop both and hold where they are
            rightMotor.move_velocity(0);
            cycleRunning = false;         // next toggle starts fresh (maxDrift check)
        } else if (done) {
            cycleRunning = false;
        }
    }

    // Queue no extra cycles: release lets the current position move finish.
    if (pressed && !cycleRunning) {
        toggleOnce();
    }
}