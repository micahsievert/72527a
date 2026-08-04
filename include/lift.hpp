#pragma once
#include "pros/motors.hpp"

#include "main.h"

enum class LiftState {
    Up,
    Down,
    Hold,
    Automatic
};

class Lift {
    public:
        Lift();
        void initialize();

        void setState(LiftState newState);
        LiftState getState() const;
        void update();

        void setTarget(double target);
        bool atTarget(double tolerance);
        double getPosition();
        void updatePositionControl();

    private:
        pros::Motor liftMotor;
        LiftState state = LiftState::Hold;
        void moveLift(double power);
        void liftHold();
        double targetPosition = 0.0;
};