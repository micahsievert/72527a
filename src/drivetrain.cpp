#include "drivetrain.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

Drivetrain::Drivetrain()
    :   leftFront(1),
        leftBack(2),
        rightFront(3),
        rightBack(4) {
}

double Drivetrain::applyDeadband(double input) {
    const double DEADBAND = 5.0;
    if (std::abs(input) < DEADBAND) {
        return 0;
    }
    return input;
}
double Drivetrain::applyCubicCurve(double input) {
    double inputCubed = input*input*input;
    double maxCubed = 127*127*127;
    double percentPower = inputCubed/maxCubed;
    double cubicPower = percentPower * 127;

    double linearPower = input;

    double cubicWeight = 5; // OK to change
    double linearWeight = 1; // OK to change
    return (cubicPower * cubicWeight + linearPower * linearWeight) / (cubicWeight + linearWeight);
}

double Drivetrain::applyQuadraticCurve(double input) {
    double inputSquared;
    if (input < 0) {
        inputSquared = input * input * -1;
    } else {
        inputSquared = input * input;
    }
    

    double maxSquared = 127 * 127;
    double percentPower = inputSquared / maxSquared;
    double quadraticPower = percentPower * 127;

    double linearPower = input;

    double quadraticWeight = 5; // OK to change
    double linearWeight = 1;   // OK to change

    return (quadraticPower * quadraticWeight + linearPower * linearWeight) / (quadraticWeight + linearWeight);
}
    

std::pair<double, double> Drivetrain::mixForwardTurn(double forward, double turn) {
    double leftPower = forward + turn;
    double rightPower = forward - turn;
    return {leftPower, rightPower};
}

std::pair<double, double> Drivetrain::normalizeMotorPowers(double left, double right) {
    double biggerValue = std::max(std::abs(left), std::abs(right));
    double normLeft;
    double normRight;
    if (biggerValue > 127.0) {
        double scaleFactor = 127.0 / biggerValue;
        normLeft = left * scaleFactor;
        normRight = right * scaleFactor;
        
    } else {
        normLeft = left;
        normRight = right;
    }
    
    return {normLeft,normRight};
}

double Drivetrain::slewify(double requestedPower, double prevPower){
    double slewn = requestedPower;
    double change = requestedPower - prevPower; // change in power - sign tells if getting more negative or more positive, number tells magnitude
    bool speedingUp = std::abs(requestedPower) >= std::abs(prevPower); // true if speeding up, false if slowing down
    double limit;

    if (speedingUp) {
        limit = slewAccel;
    } else {
        limit = slewDecel;
    }

    if (std::abs(change) > limit) { // is a slew necessary?
        if (requestedPower > prevPower) { // higher quantity of power desired
            slewn = prevPower + limit; // increase by limit
        } else { // lower quantity of power desired
            slewn = prevPower - limit; // decrease by limit
        }
    } else {
        slewn = requestedPower;
    }
    slewn = std::clamp(slewn, -127.0, 127.0);
    return slewn;
}


std::pair<double, double> Drivetrain::applySlew(double left, double right) {
    double requestedLeft = left;
    double requestedRight = right;
    double slewnLeft = slewify(requestedLeft, prevLeftPower);
    double slewnRight = slewify(requestedRight, prevRightPower);
    
    //Set currents to previous to prep for next loop
    prevLeftPower = slewnLeft;
    prevRightPower = slewnRight;

    return {slewnLeft, slewnRight};
}

void Drivetrain::setBrakeMode(BrakeMode mode) {
    switch (mode) {
        case BrakeMode::Coast:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            break;
        case BrakeMode::Brake:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            break;
        case BrakeMode::Hold:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            break;
    }
}
void Drivetrain::powerMotors(double leftPower, double rightPower) {
    leftFront.move(leftPower);
    leftBack.move(leftPower);
    rightFront.move(rightPower);
    rightBack.move(rightPower);
}



void Drivetrain::arcade(double forward, double turn) {
    //Filter out deadband values
    forward = applyDeadband(forward);
    turn = applyDeadband(turn);

    //Apply the cubic curve to incease sensitivity
    forward = applyCubicCurve(forward);
    turn = applyCubicCurve(turn);

    //Mix joysticks to get left and right motor powers
    auto [leftPower, rightPower] = mixForwardTurn(forward, turn);

    //Normalize powers to stay within range but still proportional
    auto [normLeftPower, normRightPower] = normalizeMotorPowers(leftPower, rightPower);

    //Apply slew to limit acceleration and deceleration
    auto [slewnLeftPower, slewnRightPower] = applySlew(normLeftPower, normRightPower);

    powerMotors(slewnLeftPower, slewnRightPower);
}

void Drivetrain::tank(double leftPower, double rightPower) {
    //Filter out deadband values
    leftPower = applyDeadband(leftPower);
    rightPower = applyDeadband(rightPower);

    //Apply the cubic curve to incease sensitivity
    leftPower = applyCubicCurve(leftPower);
    rightPower = applyCubicCurve(rightPower);

    //Normalize powers to stay within range but still proportional
    auto [normLeftPower, normRightPower] = normalizeMotorPowers(leftPower, rightPower);

    //Apply slew to limit acceleration and deceleration
    auto [slewnLeftPower, slewnRightPower] = applySlew(normLeftPower, normRightPower);

    powerMotors(slewnLeftPower, slewnRightPower);
}
void Drivetrain::simpleTank(double leftPower, double rightPower) {
    powerMotors(leftPower, rightPower);
}
void Drivetrain::simpleArcade(double forward, double turn) {
    auto [leftPower, rightPower] = mixForwardTurn(forward, turn);
    powerMotors(leftPower, rightPower);
}