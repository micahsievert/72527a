#pragma once
#include "pros/adi.hpp"
#include "pros/motors.hpp"





class Toggles {
public:
    Toggles(int leftMotor, int rightMotor);
    void initialize();

    
    void toggleOnce();
    void update(bool pressed);

private:
    pros::Motor leftMotor;
    pros::Motor rightMotor;
    bool targetsInitialized = false;
    bool cycleRunning = false;
    double leftTarget = 0.0;
    double rightTarget = 0.0;

};
