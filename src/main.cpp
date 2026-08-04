// Import essential libraries
#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/misc.h"
#include <cstdio>
#include <cstdint>

// Import Mechanism Classes
#include "drivetrain.hpp"
#include "intake.hpp"
#include "lift.hpp"
#include "arm.hpp"
#include "claw.hpp"
#include "macroManager.hpp"

// Instantiate Mechanism Objects
Drivetrain drivetrain;
Intake intake;
Lift lift;
Arm arm;
Claw claw('A');
MacroManager macros(arm, lift, claw);

// Set up devices for LemLib
pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({-18, -16});
pros::MotorGroup right_mg({12, 3});
pros::Imu imu_sensor(7);
pros::Rotation vertical_rotation(6);
pros::Rotation horizontal_rotation(14);
lemlib::TrackingWheel vertical_tracking(&vertical_rotation,lemlib::Omniwheel::NEW_275,-2.75); //replace this measurement
lemlib::TrackingWheel horizontal_tracking(&horizontal_rotation,lemlib::Omniwheel::NEW_275,-4.00); //replace this measurement


// LemLib setup
lemlib::OdomSensors odom_sensors(
	&vertical_tracking,
	nullptr,
	&horizontal_tracking,
	nullptr,
	&imu_sensor
);

lemlib::Drivetrain lemlibDrivetrain(
	&left_mg,
	&right_mg,
	11.5, // replace this measurement
	lemlib::Omniwheel::NEW_325,
	450, //rpm
	2 //replace this measurement
);

lemlib::ControllerSettings lateral_controller(
    10,   // kP
    0,    // kI
    30,   // kD
    3,    // anti-windup
    1,    // small error range (in)
    100,  // small error timeout (ms)
    3,    // large error range (in)
    500,  // large error timeout (ms)
    20    // slew rate
);

lemlib::ControllerSettings angular_controller(
    2,   // kP
    0,    // kI
    10,   // kD
    3,    // anti-windup
    1,    // small error range (in)
    100,  // small error timeout (ms)
    3,    // large error range (in)
    500,  // large error timeout (ms)
    0    // slew rate
);

lemlib::Chassis chassis(
	lemlibDrivetrain,
	lateral_controller,
	angular_controller,
	odom_sensors
);

void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		//pros::lcd::set_text(2, "I was pressed!");
		pros::lcd::clear_line(2);
	} else {
		pros::lcd::clear_line(2);
	}
}

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");
	pros::lcd::register_btn1_cb(on_center_button);
	
	arm.initialize();
	lift.initialize();
	//chassis.calibrate();

}

void disabled() {}
void competition_initialize() {}

void autonomous() {
	chassis.setPose(0,0,0);
	chassis.turnToHeading(90, 2000);

}

void opcontrol() { // Driver control
	while (true) {
		// Get joysticks
		int leftStick_Y = controller.get_analog(ANALOG_LEFT_Y);
		int rightStick_X = controller.get_analog(ANALOG_RIGHT_X);
		
		
		// Drive the robot
		drivetrain.arcade(leftStick_Y,rightStick_X);

		// Control the intake
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
			if (intake.getState() == IntakeState::Off) {
				intake.setState(IntakeState::In);
			} else {
				intake.setState(IntakeState::Off);
			}
		} else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
			if (intake.getState() == IntakeState::Off) {
				intake.setState(IntakeState::Out);
			} else {
				intake.setState(IntakeState::Off);
			}
		}
		intake.update(); // Update the intake state based on the current state

		
		// Control the lift
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) {
			macros.cancel();
			lift.setState(LiftState::Up);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
			macros.cancel();
			lift.setState(LiftState::Down);
		} else if (lift.getState() == LiftState::Up || lift.getState() == LiftState::Down) {
			lift.setState(LiftState::Hold); // only goes to hold if it was being manually moved
		}
		lift.update();

		// Control the arm
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
			macros.cancel();
			arm.armCustom(CustomOptions::Forward);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			macros.cancel();
			arm.armCustom(CustomOptions::Backward);
		} else if (arm.getArmState() == ArmState::Custom) {
			arm.armCustom(CustomOptions::Hold);
		}

		// Control the wrist
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
			macros.cancel();
			arm.wristCustom(CustomOptions::Forward);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
			macros.cancel();
			arm.wristCustom(CustomOptions::Backward);
		} else { // Since the driver doesn't want to change the wrist, level it to the last position
			arm.wristAutoLevel(); // Automatically level the wrist when no buttons are pressed
		}

		// Toggle the wrist
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
			macros.cancel();
			arm.toggleWristPosition();
		}
		arm.update(); // updates automatic arm positioning

		// Some macros
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
    		macros.startMatchLoad();
		} else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
			macros.startLowScore();
		}
		macros.update();
		

		// Toggle the claw open/close
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
			macros.cancel();
			if (claw.getState() == ClawState::Open) {
				claw.setState(ClawState::Closed);
			} else {
				claw.setState(ClawState::Open);
			}
		}
		claw.update();


		// Print arm/wrist encoder values
		static std::uint32_t lastDebugPrint = 0;
		if (pros::millis() - lastDebugPrint >= 200) {
			lastDebugPrint = pros::millis();

			controller.print(
				0,
				0,
				"A:%5.1f W:%5.1f",
				arm.getArmAngle(),
				arm.getWristAngle()
			);

			std::printf(
				"Arm: %.1f | Wrist: %.1f\n",
				arm.getArmAngle(),
				arm.getWristAngle()
			);
		}



		pros::delay(20);                               // Run for 20 ms then update
	}




}
