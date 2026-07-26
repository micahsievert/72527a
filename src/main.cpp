#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "pros/misc.h"

#include "drivetrain.hpp"
#include "intake.hpp"
#include "lift.hpp"
#include "arm.hpp"


Drivetrain drivetrain;
Intake intake;
Lift lift;
Arm arm;

pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({1, 2});
pros::MotorGroup right_mg({-3, -4});

pros::Imu imu_sensor(7);
pros::Rotation vertical_rotation(8);
pros::Rotation horizontal_rotation(9);

lemlib::TrackingWheel vertical_tracking(&vertical_rotation,lemlib::Omniwheel::NEW_275,-2.75); //replace this measurement
lemlib::TrackingWheel horizontal_tracking(&horizontal_rotation,lemlib::Omniwheel::NEW_275,-4.00); //replace this measurement

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
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");
	pros::lcd::register_btn1_cb(on_center_button);

	

	chassis.calibrate();

}

void disabled() {}
void competition_initialize() {}
void autonomous() {
	chassis.setPose(0,0,0);
	chassis.turnToHeading(90, 2000);

}

void opcontrol() {
	

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs
		// Get joystick values
		int leftStick_Y = controller.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int rightStick_X = controller.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		
		
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
			lift.setState(LiftState::Up);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
			lift.setState(LiftState::Down);
		} else {
			lift.setState(LiftState::Hold);
		}
		lift.update();

		// Control the arm
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
			Arm.setState(ArmState::Forward);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			Arm.setState(ArmState::Backward);
		} else {
			Arm.setState(ArmState::Hold);
		}
		Arm.update();


		pros::delay(20);                               // Run for 20 ms then update
	}




}