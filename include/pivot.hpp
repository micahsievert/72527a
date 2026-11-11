#pragma once

#include "main.hpp"
#include "pros/adi.hpp"

enum class PivotState {
    up,
    down,
    hold,
    preset
};


class Pivot {
    public:
        explicit Pivot(int motorPort, int rotationPort);
        
        void setState(PivotState newState);
        PivotState getState() const;
        
        
        void setTarget(double target);
        double getTarget() const;
        bool atTarget() const;

        double getPosition() const;

        void update();

    private:

        pros::Motor motor;
        pros::Rotation rotationSensor;

        PivotState state = PivotState::hold;
        
        double targetPosition;

        void customUpMode();
        void customDownMode();
        void holdMode();
        void presetMode();
        
};