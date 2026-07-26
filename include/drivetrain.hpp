#pragma once
#include "pros/motors.hpp"

enum class BrakeMode {
    Coast,
    Brake,
    Hold
};

class Drivetrain {
public:
    Drivetrain();

    void arcade(double forward, double turn);
    void setBrakeMode(BrakeMode mode);
    void powerMotors(double leftPower, double rightPower);
    void tank(double leftPower, double rightPower);
    void simpleTank(double leftPower, double rightPower);
    void simpleArcade(double forward, double turn);

private:
    pros::Motor leftFront;
    pros::Motor leftBack;
    pros::Motor rightFront;
    pros::Motor rightBack;

    double prevLeftPower = 0; // Don't edit
    double prevRightPower = 0; // Don't edit
    double slewAccel = 10; // OK to edit
    double slewDecel = 20; // OK to edit

    double applyDeadband(double input);
    double applyCubicCurve(double input);
    double applyQuadraticCurve(double input);
    std::pair<double, double> mixForwardTurn(double forward, double turn);
    std::pair<double, double> normalizeMotorPowers(double left, double right);
    std::pair<double, double> applySlew(double left, double right);
    double slewify(double requestedPower, double prevPower);

    
};