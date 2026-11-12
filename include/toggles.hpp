#pragma once
#include "pros/adi.hpp"
#include "pros/motors.hpp"





class Toggles {
public:
    Toggles(int leftMotor, int rightMotor);
    void initialize();

    
    void toggleOnce();
    // Blocks until both motors reach their cycle targets.
    void waitUntilDone();
    void update(bool pressed);

private:
    pros::Motor leftMotor;
    pros::Motor rightMotor;
    bool targetsInitialized = false;
    bool cycleRunning = false;
    double leftTarget = 0.0;
    double rightTarget = 0.0;

};
