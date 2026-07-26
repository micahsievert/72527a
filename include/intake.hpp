#pragma once
#include "pros/motors.hpp"

#include "main.h"

enum class IntakeState {
    In,
    Out,
    Off
};

class Intake {
    public:
        Intake();

        void setState(IntakeState newState);
        IntakeState getState() const;
        void update();

    private:
        pros::Motor intakeMotor;
        IntakeState state = IntakeState::Off;
        void intakeIn();
        void intakeOut();
        void intakeOff();
};