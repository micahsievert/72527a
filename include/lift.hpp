#pragma once
#include "pros/motors.hpp"

#include "main.h"

enum class LiftState {
    Up,
    Down,
    Hold
};

class Lift {
    public:
        Lift();

        void setState(LiftState newState);
        LiftState getState() const;
        void update();

    private:
        pros::Motor liftMotor;
        LiftState state = LiftState::Hold;
        void liftUp();
        void liftDown();
        void liftHold();
};