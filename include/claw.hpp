#pragma once

#include "main.hpp"
#include "pros/adi.hpp"

enum class ClawState {
    grabbing,
    releasing,
    off
};

class Claw {
    public:
        explicit Claw(int motorPort);
        void setState(ClawState newState);
        ClawState getState() const;
        void update();

    private:

        pros::Motor motor;
        ClawState state = ClawState::off;
        
        void grabbingMode();
        void releasingMode();
        void offMode();
        
};