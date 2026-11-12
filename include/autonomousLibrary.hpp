#pragma once
#include <cstdint>

// Drivetrain helpers
// Motor rotations (not wheel rotations). powqer: -127 to 127, negative reverse.
// Negative rotations flip the power direction; zero power stops and returns false
// unless zero rotations were requested.
// Blocks until all motors arrive or timeout_ms expires, then brakes.
// Returns false on invalid input, motor error, or timeout. Use with chassis idle.
bool driveRotations(double rotations, int powqer = 100,
                    std::uint32_t timeout_ms = 5000);
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
