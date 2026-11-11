#include "drivetrain.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

Drivetrain::Drivetrain(int leftFront, int leftMiddle, int leftBack, int rightFront, int rightMiddle, int rightBack, char shifterSolenoidPort)
    :   leftFront(leftFront),
        leftMiddle(leftMiddle),
        leftBack(leftBack),
        rightFront(rightFront),
        rightMiddle(rightMiddle),
        rightBack(rightBack),
        shifter(shifterSolenoidPort)    {
}

void Drivetrain::initialize() {
    // Configure startup state without delaying or moving the drive motors.
    state = ShifterState::highSpeed;
    shifter.set_value(true);
    setBrakeMode(BrakeMode::Coast);
    powerMotors(0, 0);
}


// Shifter Control
void Drivetrain::jiggleRobot() {
    double timeInEachDirection = 10; // milliseconds
    double numOscilations = 4;
    double oscilationPower = 50;

    for (int i = 0; i < numOscilations; i++) {
        powerMotors(oscilationPower, oscilationPower);
        pros::delay(static_cast<std::uint32_t>(timeInEachDirection));
        powerMotors(-oscilationPower, -oscilationPower);
        pros::delay(static_cast<std::uint32_t>(timeInEachDirection));
    }
}

void Drivetrain::setShifterState(ShifterState newState) {
    state = newState;
    if (newState == ShifterState::highSpeed) {
        shifter.set_value(true);
    } else if (newState == ShifterState::highTorque) {
        shifter.set_value(false);
    }
    jiggleRobot();
}
ShifterState Drivetrain::getShifterState() const {
    return state;
}
void Drivetrain::toggleShifter() {
    ShifterState currentState = getShifterState();

    if (currentState == ShifterState::highSpeed) {
        setShifterState(ShifterState::highTorque);
    } else if (currentState == ShifterState::highTorque) {
        setShifterState(ShifterState::highSpeed);
    }
}

// Get RPM
double Drivetrain::getRPM() {
    // Example implementation, replace with actual RPM calculation
    return leftFront.get_actual_velocity();
}






// Drivetrain Power Control Supplements / Accessories
double Drivetrain::applyDeadband(double input) {
    const double DEADBAND = 5.0;
    if (std::abs(input) < DEADBAND) { return 0; } 
    else { return input; }
}

double Drivetrain::applyCubicPowerRamp(double input) {
    double inputCubed = input*input*input;
    double maxCubed = 127*127*127;
    double percentPower = inputCubed/maxCubed;
    double cubicPower = percentPower * 127;
    double linearPower = input;

    double cubicWeight = 5; // OK to change
    double linearWeight = 1; // OK to change
    return (cubicPower * cubicWeight + linearPower * linearWeight) / (cubicWeight + linearWeight);
}

double Drivetrain::applyQuadraticPowerRamp(double input) {
    double inputSquared;
    if (input < 0) { inputSquared = input * input * -1; } 
    else { inputSquared = input * input; }

    double maxSquared = 127 * 127;
    double percentPower = inputSquared / maxSquared;
    double quadraticPower = percentPower * 127;
    double linearPower = input;

    double quadraticWeight = 5; // OK to change
    double linearWeight = 1;   // OK to change
    return (quadraticPower * quadraticWeight + linearPower * linearWeight) / (quadraticWeight + linearWeight);
}
    
std::pair<double, double> Drivetrain::mixForwardAndTurn(double forward, double turn) {
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


void Drivetrain::setBrakeMode(BrakeMode mode) {
    switch (mode) {
        case BrakeMode::Coast:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            leftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            break;
        case BrakeMode::Brake:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            leftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            rightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
            break;
        case BrakeMode::Hold:
            leftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            leftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            leftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            rightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            break;
    }
}

void Drivetrain::powerMotors(double leftPower, double rightPower) {
    leftFront.move(leftPower);
    leftMiddle.move(leftPower);
    leftBack.move(leftPower);
    rightFront.move(rightPower);
    rightMiddle.move((rightPower));
    rightBack.move(rightPower);
}


void Drivetrain::arcade(double forward, double turn) {
    //Filter out deadband values
    forward = applyDeadband(forward);
    turn = applyDeadband(turn);

    // //Apply the cubic curve to incease sensitivity
    // forward = applyCubicPowerRamp(forward);
    // turn = applyCubicPowerRamp(turn);

    //Mix joysticks to get left and right motor powers
    auto [leftPower, rightPower] = mixForwardAndTurn(forward, turn);

    // //Normalize powers to stay within range but still proportional
    // auto [normLeftPower, normRightPower] = normalizeMotorPowers(leftPower, rightPower);

    

    powerMotors(leftPower, rightPower);
}

void Drivetrain::tank(double leftPower, double rightPower) {
    //Filter out deadband values
    leftPower = applyDeadband(leftPower);
    rightPower = applyDeadband(rightPower);

    // //Apply the cubic curve to incease sensitivity
    // leftPower = applyCubicCurve(leftPower);
    // rightPower = applyCubicCurve(rightPower);

    // //Normalize powers to stay within range but still proportional
    // auto [normLeftPower, normRightPower] = normalizeMotorPowers(leftPower, rightPower);


    powerMotors(leftPower, rightPower);
}

void Drivetrain::simpleTank(double leftPower, double rightPower) {
    powerMotors(leftPower, rightPower);
}

void Drivetrain::simpleArcade(double forward, double turn) {
    auto [leftPower, rightPower] = mixForwardAndTurn(forward, turn);
    powerMotors(leftPower, rightPower);
}