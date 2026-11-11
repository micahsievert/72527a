#pragma once

// Drivetrain helpers
void driveTimed(int power, std::uint32_t duration);
void startDriving(int power);
void drivetrainBrake();

// Timing
void wait(std::uint32_t duration_ms);

// Mechanism helpers
void moveLiftAndPivot(
    double liftTarget,
    double pivotTarget,
    std::uint32_t timeout
);

void moveLiftTo(
    double target,
    std::uint32_t timeout
);

void movePivotTo(
    double target,
    std::uint32_t timeout
);

// Sensor helpers
bool waitForLine(std::uint32_t timeout);