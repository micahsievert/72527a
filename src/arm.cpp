#include "arm.hpp"
#include <cmath>

Arm::Arm()
    : armMotor(8),
      wristMotor(21) {
    }

void Arm::initialize() {
    armMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    wristMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    armMotor.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
    wristMotor.set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES);
    
    armMotor.tare_position();
    wristMotor.tare_position();

    targetArmAngle = 0;
}

void Arm::setArmState(ArmState newState) {
    armState = newState;
}
ArmState Arm::getArmState() const {
    return armState;
}

void Arm::armMove(double power) {
    armMotor.move(power);
}
void Arm::armHold() {
    armMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    armMotor.brake();
}
void Arm::armCustom(CustomOptions option) {
    armState = ArmState::Custom;
    double power = 80;
    switch (option) {
        case CustomOptions::Forward:
            armMove(power);
            break;
        case CustomOptions::Backward:
            armMove(-power);
            break;
        case CustomOptions::Hold:
            armHold();
            break;
    }
}

void Arm::wristMove(double power) {
    wristMotor.move(power);
}
void Arm::wristHold() {
    wristMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    wristMotor.brake();
}
void Arm::wristCustom(CustomOptions option) {
    wristLevelingActive = false; // Disable wrist leveling when in custom mode
    double power = 30;
    switch (option) {
        case CustomOptions::Forward:
            wristMove(power);
            break;
        case CustomOptions::Backward:
            wristMove(-power);
            break;
        case CustomOptions::Hold:
            wristHold();
            break;
    }
}

double Arm::getArmAngle() { 
    return armMotor.get_position(); 
}
double Arm::getWristAngle() { 
    return wristMotor.get_position(); 
}
void Arm::setArmTarget(double armTarget) {
    targetArmAngle = armTarget;
    armState = ArmState::Automatic;
}

bool Arm::armAtTarget(double tolerance) {
    double error = targetArmAngle - getArmAngle();
    return std::abs(error) <= tolerance;
}
bool Arm::wristAtTarget(double tolerance) {
    double error = getWristAutoLevelTarget() - getWristAngle();
    return std::abs(error) <= tolerance;
}
bool Arm::isWristInIntakePosition() const {
    return wristInIntakePosition;
}

double Arm::getWristAutoLevelTarget() {
    double armGearingRatio = 12.0 / 60.0; // How many physical degrees come out of each arm motor degree?
    double vBarGearingRatio = 12.0 / 18.0; // How many physical degrees come out of each wrist motor degree?
    double gearingCompensationRatio = armGearingRatio / vBarGearingRatio;
                // How many degrees did the arm rotate? Divide that by how many real wrist degrees i get per 1 motor degree

    return wristLevelOffset 
        + (getArmAngle() * gearingCompensationRatio) // strip arm movement out of leveling offset
        + wristOrientationOffset; // strip any offset due to intake or matchload position
}

void Arm::setWristLevelPosition() {
    wristInIntakePosition = false; // Set the wrist to the level position
    
    wristLevelOffset = LEVEL_ORIENTATION_OFFSET; // Reset any offset for intake or matchload positions
    wristOrientationOffset = 0.0; // Reset any offset for intake or matchload positions
    wristLevelingActive = true; // Enable wrist leveling
}

void Arm::setWristIntakePosition() {
    wristInIntakePosition = true; // Set the wrist to the intake position
    
    wristLevelOffset = LEVEL_ORIENTATION_OFFSET; // Reset any offset for intake or matchload positions
    wristOrientationOffset = INTAKE_ORIENTATION_OFFSET; // Set the offset for the intake position
    wristLevelingActive = true; // Enable wrist leveling
}

void Arm::toggleWristPosition() {
    if (wristInIntakePosition) {
        setWristLevelPosition();
    } else {
        setWristIntakePosition();
    }
}

void Arm::updateArmPositioning() {
    double kP = 1;
    double armAngle = getArmAngle();
    double error = targetArmAngle - armAngle;
    if (error < 0.5 && error > -0.5) { // wrist is leveled, stop moving
        armHold();
    } else {
        double maxPower = 30; // Maximum power limit
        if (error * kP > maxPower) {
            armMove(maxPower);
        } else if (error * kP < -maxPower) {
            armMove(-maxPower);
        } else {
            armMove(error * kP);
        }
    }
}



void Arm::wristAutoLevel() {
    
    double armGearingRatio = 12.0 / 60.0; // How many physical degrees come out of each arm motor degree?
    double vBarGearingRatio = 12.0 / 18.0; // How many physical degrees come out of each wrist motor degree?
    double gearingCompensationRatio = armGearingRatio / vBarGearingRatio;
                // How many degrees did the arm rotate? Divide that by how many real wrist degrees i get per 1 motor degree

    if (wristLevelingActive == false) { // wrist was just released, so check where it was left
        wristLevelOffset = // capture the world-relative desired orientation of the wrist
            getWristAngle() // whats the current wrist motor position?
            - (getArmAngle() * gearingCompensationRatio) // remove rotation due to arm movement
            - wristOrientationOffset; // remove any offset due to intake or matchload position
        wristLevelingActive = true; // enable wrist leveling so offset is maintained
    }

    double autoLevelTarget = getWristAutoLevelTarget();

    double error = autoLevelTarget - getWristAngle();
    double kP = 1.0;
    if (error < 0.5 && error > -0.5) { // wrist is leveled, stop moving
        wristHold();
    } else {
        wristMove(error * kP);
    }
}

void Arm::update() {
    if (armState != ArmState::Custom) {
        updateArmPositioning();
    }
    // wrist auto-level is always called in opcontrol unless custom is in progress
}




// Old wrist leveling code, not used anymore
// void Arm::updateWristLeveling() {
//     // Calculate currents vs. targets
//     double armAngle = getArmAngle();
//     double wristAngle = wristMotor.get_position();
//     double targetWristAngle = armAngle * 0.2 * 2; // 0.2 to compensate for 1:5 arm, 0.5 for v-bar ratio
//     if (isHorizontal == false) { // claw is vertical for grabbing from intake
//         targetWristAngle += 90; // 90 degrees to compensate for claw being vertical
//     }
//     double error = targetWristAngle - wristAngle;