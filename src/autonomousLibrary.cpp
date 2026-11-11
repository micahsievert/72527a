#include "autonomousLibrary.hpp"

#include "main.hpp"
#include "lift.hpp"
#include "pivot.hpp"

#include "pros/rtos.hpp"

// Bring over things from main.cpp
extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;

extern Lift lift;
extern Pivot pivot;

extern pros::adi::AnalogIn lineSensor;


// ========================================
// DRIVETRAIN HELPERS
// ========================================

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

void moveLiftAndPivot(double liftTarget, double pivotTarget, std::uint32_t timeout) {
    lift.setTarget(liftTarget);
    pivot.setTarget(pivotTarget);

    const std::uint32_t startTime = pros::millis();

    while (
        (!lift.atTarget() || !pivot.atTarget()) &&
        pros::millis() - startTime < timeout
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