#pragma once
#include "arm.hpp"
#include "pros/motors.hpp"

#include "main.h"

enum class ArmState {
    Forward,
    Backward,
    Hold
};

class Arm {
    public:
        Arm();

        void setState(ArmState newState);
        ArmState getState() const;
        void update();

    private:
        pros::Motor armMotor;
        ArmState state = ArmState::Hold;
        void armForward();
        void armBackward();
        void armHold();
};