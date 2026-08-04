#pragma once

#include "main.h"
#include "pros/adi.hpp"

enum class ClawState {
    Open,
    Closed,
};

class Claw {
    public:
        explicit Claw(std::uint8_t solenoidPort);
        void setState(ClawState newState);
        ClawState getState() const;
        void update();

    private:

        pros::adi::DigitalOut solenoid;
        ClawState state = ClawState::Open;
        
};