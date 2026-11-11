#pragma once

#include "main.hpp"
#include "pros/adi.hpp"

enum class LiftState {
    up,
    down,
    hold,
    preset
};

class Lift {
    public:
        explicit Lift(int motorPort, int rotationPort);
        void setState(LiftState newState);
        LiftState getState() const;
        
        
        // Caps preset targets at 1700 degrees without limiting manual movement.
        void setTarget(double target);
        void holdCurrentPosition(); // Capture this position as the PD target.
        double getTarget() const;
        bool atTarget() const;

        double getPosition() const;

        void update();

    private:

        pros::Motor motor;
        pros::Rotation rotationSensor;

        LiftState state = LiftState::hold;
        
        double targetPosition = 0.0;

        void customUpMode();
        void customDownMode();
        void holdMode();
        void presetMode();
        
};
