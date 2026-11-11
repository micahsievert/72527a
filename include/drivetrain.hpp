#pragma once
#include "pros/adi.hpp"
#include "pros/motors.hpp"

enum class BrakeMode {
    Coast,
    Brake,
    Hold
};

enum class ShifterState {
    highTorque,
    highSpeed
};

class Drivetrain {
public:
    Drivetrain(int left1, int left2, int left3, int right1, int right2, int right3, char shifterSolenoidPort);
    void initialize();

    void arcade(double forward, double turn);
    void setBrakeMode(BrakeMode mode);
    void powerMotors(double leftPower, double rightPower);
    void tank(double leftPower, double rightPower);
    void simpleTank(double leftPower, double rightPower);
    void simpleArcade(double forward, double turn);

    void setShifterState(ShifterState);
    ShifterState getShifterState() const;
    void toggleShifter();
    void update();

    void jiggleRobot();
    double getRPM();

private:
    pros::Motor leftFront;
    pros::Motor leftMiddle;
    pros::Motor leftBack;
    pros::Motor rightFront;
    pros::Motor rightMiddle;
    pros::Motor rightBack;
    pros::adi::DigitalOut shifter;

    ShifterState state = ShifterState::highSpeed;

    double prevLeftPower = 0; // Don't edit
    double prevRightPower = 0; // Don't edit
    double slewAccel = 10; // OK to edit
    double slewDecel = 20; // OK to edit

    double applyDeadband(double input);
    double applyCubicPowerRamp(double input);
    double applyQuadraticPowerRamp(double input);
    std::pair<double, double> mixForwardAndTurn(double forward, double turn);
    std::pair<double, double> normalizeMotorPowers(double left, double right);
    

    
};
