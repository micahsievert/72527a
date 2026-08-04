#pragma once
#include "pros/motors.hpp"

#include "main.h"

enum class ArmState {
    Matchload,
    LowScoring,
    HighScoring,
    Intake,
    Custom,
    StartPosition,
    Automatic
};

enum class CustomOptions {
    Forward,
    Backward,
    Hold
};

class Arm {
    public:
        Arm();
        void initialize();
        void update();


        void setArmState(ArmState newState);
        ArmState getArmState() const;
        

        void armMove(double power);
        void armHold();
        void armCustom(CustomOptions option);

        void wristMove(double power);
        void wristHold();
        void wristCustom(CustomOptions option);

        void toggleWristPosition();


        void wristAutoLevel();
        double getArmAngle();
        double getWristAngle();
        void setArmTarget(double armTarget);

        bool armAtTarget(double tolerance);
        bool wristAtTarget(double tolerance);

        void setWristLevelPosition();
        void setWristIntakePosition();
        bool isWristInIntakePosition() const;

        
        

    private:
        pros::Motor armMotor;
        pros::Motor wristMotor;

        ArmState armState = ArmState::StartPosition;
        
        double targetArmAngle;

        double getWristAutoLevelTarget();

        void updateArmPositioning();

        
        bool wristLevelingActive = false;
        bool wristInIntakePosition = false;
        
        double wristLevelOffset = 0.0; // keeps track of the driver's desired phyiscal orientation
        double wristOrientationOffset = 0.0; //
        static constexpr double LEVEL_ORIENTATION_OFFSET = -0.0;
        static constexpr double INTAKE_ORIENTATION_OFFSET = 135.0;
        
};
