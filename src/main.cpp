// Import essential libraries
#include "main.hpp"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "mvlib/core.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/misc.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include "pros/adi.hpp"
#include "mvlib/api.hpp"
#include "mvlib/Optional/lemlib.hpp"


// Import Mechanism Classes
#include "drivetrain.hpp"
#include "lift.hpp"
#include "pivot.hpp"
#include "claw.hpp"
#include "toggles.hpp"
#include "macroManager.hpp"
#include "autonomousLibrary.hpp"

// Import Jerry Paths
ASSET(figure8_txt);

// Instantiate Mechanism Objects -- BRAIN PORTS
const int leftFrontPort = -3; // negative port
const int leftMiddlePort = 4; // negative port
const int leftBackPort = -5; // negative port
const int rightFrontPort = 8;
const int rightMiddlePort = -7;
const int rightBackPort = 6;

const char shifterSolenoidPort = 'A';

const int liftMotorPort = 15;
const int liftRotationPort = -13; // negative port

const int pivotMotorPort = -16; // negative port
const int pivotRotationPort = 18;

const int clawMotorPort = -19; // negative port

const int leftTogglePort = 14;
const int rightTogglePort = -21; // negative port

const int interialPort = 1;
const int verticalRotationPort = 9;
const int horizontalRotationPort = -10; // negative port



Drivetrain drivetrain(leftFrontPort, leftMiddlePort, leftBackPort, rightFrontPort, rightMiddlePort, rightBackPort, shifterSolenoidPort);
Lift lift(liftMotorPort, liftRotationPort);
Pivot pivot(pivotMotorPort, pivotRotationPort);
Claw claw(clawMotorPort);
Toggles toggles(leftTogglePort, rightTogglePort);
MacroManager macros(pivot, lift, claw);

// Set up devices for LemLib & our own code
pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({leftFrontPort, leftMiddlePort, leftBackPort}, pros::MotorGearset::green);
pros::MotorGroup right_mg({rightFrontPort, rightMiddlePort, rightBackPort}, pros::MotorGearset::green);
pros::Imu imu_sensor(interialPort); 
pros::Rotation vertical_rotation(verticalRotationPort);
pros::Rotation horizontal_rotation(horizontalRotationPort);
pros::adi::AnalogIn lineSensor('G');
lemlib::TrackingWheel vertical_tracking(&vertical_rotation,1.97,-3.25); 
lemlib::TrackingWheel horizontal_tracking(&horizontal_rotation,1.97,4.156);


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
	12.88, // replace this measurement
	lemlib::Omniwheel::NEW_325,
	450, //rpm
	2 //replace this measurement
);

lemlib::ControllerSettings lateral_controller(
    17,   // kP
    0,    // kI
    60,   // kD
    3,    // anti-windup
    1,    // small error range (in)
    100,  // small error timeout (ms)
    3,    // large error range (in)
    500,  // large error timeout (ms)
    20    // slew rate
);

lemlib::ControllerSettings angular_controller(
    4,   // kP
    0,    // kI
    33,   // kD
    1.5,    // anti-windup
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

void initialize() { // INIT
	pros::lcd::initialize();
	drivetrain.initialize();
	toggles.initialize();
	pros::lcd::register_btn1_cb(on_center_button);
	chassis.calibrate();
	chassis.setPose(0, 0, 0);
}

void disabled() {}
void competition_initialize() {}

// Macro Variables --- VARS --- in degrees, from brain screen
		double pivotFlatPosition = 384;
		double pivotLoaderPosition = 88;
		double pivotReleasePosition = 600;
		
		double liftScoringPosition = 900;
		double liftLoaderPosition = 364.3;
		double liftGroundGrabPosition = 465;
		double liftUpwardMovementOnRelease = 500;
		double liftTargetAfterScore = 10000;
		double liftSinglePinScorePosition = 210;


// AUTONOMOUS // // AUTONOMOUS // AUTONOMOUS // AUTONOMOUS //
// AUTONOMOUS // // AUTONOMOUS // AUTONOMOUS // AUTONOMOUS //
// AUTONOMOUS // // AUTONOMOUS // AUTONOMOUS // AUTONOMOUS //
// AUTONOMOUS // // AUTONOMOUS // AUTONOMOUS // AUTONOMOUS //

void autonomous() {
	// drivetrain.setBrakeMode(BrakeMode::Hold);
	// chassis.moveToPoint(10, 10, 10000);
	// wait(2000);
	// chassis.moveToPoint(10, 10, 10000);
    // chassis.waitUntilDone();

	toggles.toggleOnce();
	wait(1000);
	toggles.toggleOnce();
	wait(1000);
	moveLiftAndPivot(210, pivotFlatPosition, 3000);

	chassis.turnToHeading(48, 3000);
	chassis.waitUntilDone();

	drivetrain.powerMotors(63,  63);
	wait(2000);
	drivetrain.powerMotors(0, 0);

	claw.setState(ClawState::releasing);

}

void opcontrol() { // Driver control
	lift.holdCurrentPosition();
	pivot.holdCurrentPosition();
	while (true) {
		macros.update();
		
		// Get joysticks
		int leftStick_Y = controller.get_analog(ANALOG_LEFT_Y);
		int rightStick_X = controller.get_analog(ANALOG_RIGHT_X);

		
		// Drive the robot
		drivetrain.simpleArcade(leftStick_Y, rightStick_X);

		// Control the shifter
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
			drivetrain.toggleShifter();
		}

		
		// Control the lift
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) {
			macros.cancel();
			lift.setState(LiftState::up);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
			macros.cancel();
			lift.setState(LiftState::down);
		} else if (lift.getState() == LiftState::up || lift.getState() == LiftState::down) { // In up or down mode, but no longer pressing the button
			lift.holdCurrentPosition(); // Hold where the manual button was released
		}
		lift.update();

		
		// Control the pivot
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			macros.cancel();
			pivot.setState(PivotState::up);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
			macros.cancel();
			pivot.setState(PivotState::down);
		} else if (pivot.getState() == PivotState::up || pivot.getState() == PivotState::down) { // In up or down mode, but no longer pressing the button
			pivot.holdCurrentPosition(); // Hold where the manual button was released
		}
		pivot.update(); // updates automatic pivot positioning

		
		// Control the claw
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) { // the grab/off toggle
			//macros.cancel();
			if (claw.getState() == ClawState::grabbing) {
				claw.setState(ClawState::off);
			} else {
				claw.setState(ClawState::grabbing);
			}
		} else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) { // the release/off toggle
			//macros.cancel();
			if (claw.getState() == ClawState::releasing) {
				claw.setState(ClawState::off);
			} else {
				claw.setState(ClawState::releasing);
			}
		}
		claw.update(); // Apply button and macro commands to the motor every loop.

		
		
		
		
		// Macro Configuration

		// Scoring Position	
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
			macros.start(liftScoringPosition,pivotFlatPosition,ClawState::grabbing);
		
		// Match Loader Position
		} else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
			macros.start(liftLoaderPosition, pivotLoaderPosition, ClawState::grabbing);
		

		// Score/Release the cup/pin
		} else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
			macros.start(lift.getPosition()+50, pivotReleasePosition, ClawState::releasing);
		}

		// Hold X to repeat full cycles; release finishes the current cycle.
		toggles.update(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X));

		
		

		

		// Print arm/lift encoder values
		static std::uint32_t lastDebugPrint = 0;
		if (pros::millis() - lastDebugPrint >= 100) {
			lastDebugPrint = pros::millis();
			
			lemlib::Pose pose = chassis.getPose();
			pros::lcd::print(0, "X: %.1f Y: %.1f H: %.1f",
				pose.x, pose.y, pose.theta);

			const double liftPosition = lift.getPosition();
			if (std::isfinite(liftPosition)) {
				pros::lcd::print(1, "Lift: %.1f / %.1f %s",
					liftPosition, lift.getTarget(),
					lift.atTarget() ? "READY" : "MOVING");
			} else {
				pros::lcd::print(1, "Lift: SENSOR ERR P%d / %.1f",
					std::abs(liftRotationPort), lift.getTarget());
			}

			pros::lcd::print(2, "Pivot: %.1f / %.1f %s",
				pivot.getPosition(), pivot.getTarget(),
				pivot.atTarget() ? "READY" : "MOVING");
			
			ClawState clawState = claw.getState();
			pros::lcd::print(3, "Claw: %s",
				clawState == ClawState::grabbing ? "GRABBING" :
				clawState == ClawState::releasing ? "RELEASING" :
				"OFF");

			pros::lcd::print(4, "Drive: %s | RPM: %.0f",
				drivetrain.getShifterState() == ShifterState::highSpeed
					? "SPEED" : "TORQUE",
				drivetrain.getRPM());
			
		} // close debug print block
		pros::delay(20); // Yield on every driver-control iteration.
	} // close master while true loop	
}// close operator control function
