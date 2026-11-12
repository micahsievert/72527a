#include "autonomousLibrary.hpp"

#include "main.hpp"
#include "lift.hpp"
#include "pivot.hpp"

#include "pros/rtos.hpp"
#include <cmath>

// Bring over things from main.cpp
extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;

extern Lift lift;
extern Pivot pivot;

extern pros::adi::AnalogIn lineSensor;


// ========================================
// DRIVETRAIN HELPERS
// ========================================

bool driveRotations(double rotations, int powqer, std::uint32_t timeout_ms) {
    const double degrees = std::abs(rotations) * 360.0;
    if (!std::isfinite(degrees) || powqer < -127 || powqer > 127 ||
        timeout_ms == 0) {
        drivetrainBrake();
        return false;
    }
    if (rotations == 0.0 || powqer == 0) {
        drivetrainBrake();
        return rotations == 0.0;
    }

    if (left_mg.set_encoder_units_all(pros::E_MOTOR_ENCODER_DEGREES) != 1 ||
        right_mg.set_encoder_units_all(pros::E_MOTOR_ENCODER_DEGREES) != 1) {
        drivetrainBrake();
        return false;
    }

    const auto leftStart = left_mg.get_position_all();
    const auto rightStart = right_mg.get_position_all();
    const auto valid = [](const std::vector<double>& positions) {
        if (positions.empty()) return false;
        for (double position : positions) {
            if (!std::isfinite(position)) return false;
        }
        return true;
    };
    if (!valid(leftStart) || !valid(rightStart)) {
        drivetrainBrake();
        return false;
    }

    // Negative rotations reverse the requested power, preserving driveRotations(-1).
    const int command = rotations < 0 ? -powqer : powqer;
    const int direction = command > 0 ? 1 : -1;
    const auto reached = [&](const std::vector<double>& positions,
                             const std::vector<double>& start) {
        if (positions.size() != start.size()) return false;
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if ((positions[i] - start[i]) * direction < degrees) return false;
        }
        return true;
    };

    if (left_mg.move(command) != 1 || right_mg.move(command) != 1) {
        drivetrainBrake();
        return false;
    }
    bool leftDone = false;
    bool rightDone = false;
    const std::uint32_t startTime = pros::millis();
    while (pros::millis() - startTime < timeout_ms) {
        const auto leftPositions = left_mg.get_position_all();
        const auto rightPositions = right_mg.get_position_all();
        if (!valid(leftPositions) || !valid(rightPositions)) break;
        if (!leftDone && reached(leftPositions, leftStart)) {
            left_mg.brake();
            leftDone = true;
        }
        if (!rightDone && reached(rightPositions, rightStart)) {
            right_mg.brake();
            rightDone = true;
        }
        if (leftDone && rightDone) {
            drivetrainBrake();
            return true;
        }
        lift.update();
        pivot.update();
        pros::delay(10);
    }
    drivetrainBrake();
    return false;
}

void driveTimed(int power, std::uint32_t duration) {
    left_mg.move(power);
    right_mg.move(power);

    pros::delay(duration);

    drivetrainBrake();
}

void startDriving(int power) {
    left_mg.move(power);
    right_mg.move(power);
}

void drivetrainBrake() {
    left_mg.brake();
    right_mg.brake();
}


// ========================================
// TIMING
// ========================================

void wait(std::uint32_t duration_ms) {
    pros::delay(duration_ms);
}


// ========================================
// MECHANISM HELPERS
// ========================================

void moveLiftAndPivot(double liftTarget, double pivotTarget, std::uint32_t timeoutMilliSeconds) {
    lift.setTarget(liftTarget);
    pivot.setTarget(pivotTarget);

    const std::uint32_t startTime = pros::millis();

    while (
        (!lift.atTarget() || !pivot.atTarget()) &&
        pros::millis() - startTime < timeoutMilliSeconds
    ) {
        lift.update();
        pivot.update();

        pros::delay(10);
    }
}


void moveLiftTo(
    double target,
    std::uint32_t timeout
) {
    lift.setTarget(target);

    const std::uint32_t startTime = pros::millis();

    while (
        !lift.atTarget() &&
        pros::millis() - startTime < timeout
    ) {
        lift.update();
        pivot.update();

        pros::delay(10);
    }
}


void movePivotTo(
    double target,
    std::uint32_t timeout
) {
    pivot.setTarget(target);

    const std::uint32_t startTime = pros::millis();

    while (
        !pivot.atTarget() &&
        pros::millis() - startTime < timeout
    ) {
        lift.update();
        pivot.update();

        pros::delay(10);
    }
}


// ========================================
// SENSOR HELPERS
// ========================================

bool waitForLine(std::uint32_t timeout) {
    const std::uint32_t startTime = pros::millis();

    int consecutiveDetections = 0;

    while (pros::millis() - startTime < timeout) {
        if (lineSensor.get_value() < 1000) {
            consecutiveDetections++;

            if (consecutiveDetections >= 3) {
                return true;
            }
        } else {
            consecutiveDetections = 0;
        }

        // Continue updating mechanisms
        // while waiting for the sensor.
        lift.update();
        pivot.update();

        pros::delay(5);
    }

    return false;
}
