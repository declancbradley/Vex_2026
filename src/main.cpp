#include "main.h"


// Chassis constructor
ez::Drive chassis(
    // These are your drive motors, the first motor is used for sensing!
    {9, 19, 17},     // Left Chassis Ports (negative port will reverse it!)
    {-2, -13, -15},  // Right Chassis Ports (negative port will reverse it!)

    11,      // IMU Port (inertial sensor, crucial)
    3.25,  // Wheel Diameter (Remember, 4" wheels without screw holes are actually 4.125!)
    200);   // Wheel RPM = cartridge * (motor gear / wheel gear)


void initialize() {
  
  pros::delay(500);  // Stop the user from doing anything while legacy ports configure

  // Configure your chassis controls
  chassis.opcontrol_curve_buttons_toggle(true);   // Enables modifying the controller curve with buttons on the joysticks
  chassis.opcontrol_drive_activebrake_set(2.0);   // Sets the active brake kP. We recommend ~2.  0 will disable.
  chassis.opcontrol_curve_default_set(0.0, 0.0);  // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)

  // Set the drive to your own constants from autons.cpp!
  default_constants();

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.autons_add({
    {"Match\n\nUse for competitions", match_auton}, 
    {"Auton Skills\n\nUse for competitions", skills_auton}  
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();
  master.rumble(chassis.drive_imu_calibrated() ? "." : "---");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 **/
void competition_initialize() {
  // . . .
}

void autonomous() {
  chassis.pid_targets_reset();                // Resets PID targets to 0
  chassis.drive_imu_reset();                  // Reset gyro position to 0
  chassis.drive_sensor_reset();               // Reset drive sensors to 0
  chassis.odom_xyt_set(0_in, 0_in, 0_deg);    // Set the current position, you can start at a specific position with this
  chassis.drive_brake_set(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency

  ez::as::auton_selector.selected_auton_call();  // Calls selected auton from autonomous selector
}

/**
 * Only runs when not connected to competition switch, for debug purposes
 */
void ez_screen_task() {
  while (true) {
    if (!pros::competition::is_connected()) {
      // Blank page for odom debugging
      if (chassis.odom_enabled() && !chassis.pid_tuner_enabled()) {
        // If we're on the first blank page...
        if (ez::as::page_blank_is_on(0)) {
          // Display X, Y, and Theta
          ez::screen_print("x: " + util::to_string_with_precision(chassis.odom_x_get()) +
                               "\ny: " + util::to_string_with_precision(chassis.odom_y_get()) +
                               "\na: " + util::to_string_with_precision(chassis.odom_theta_get()),
                           1);  // Don't override the top Page line
        }
      }
    }

    // Remove all blank pages when connected to a comp switch
    else {
      if (ez::as::page_blank_amount() > 0)
        ez::as::page_blank_remove_all();
    }

    pros::delay(ez::util::DELAY_TIME);
  }
}
//pros::Task ezScreenTask(ez_screen_task);

/**
 * - open PID tuner by pressing X in opcontrol
 * - run autonomous by pressing DOWN+B together in opcontrol
 */
void ez_template_extras() {
  // Only run this when not connected to a competition switch
  if (!pros::competition::is_connected()) {
    // PID Tuner
    // - after you find values that you're happy with, you'll have to set them in auton.cpp

    // Enable / Disable PID Tuner
    //  When enabled:
    //  * use A and Y to increment / decrement the constants
    //  * use the arrow keys to navigate the constants
    if (master.get_digital_new_press(DIGITAL_X))
      chassis.pid_tuner_toggle();

    // Trigger the selected autonomous routine
    if (master.get_digital(DIGITAL_B) && master.get_digital(DIGITAL_DOWN)) {
      pros::motor_brake_mode_e_t preference = chassis.drive_brake_get();
      autonomous();
      chassis.drive_brake_set(preference);
    }
    // Allow PID Tuner to iterate
    chassis.pid_tuner_iterate();
  }
  // Disable PID Tuner when connected to a comp switch
  else {
    if (chassis.pid_tuner_enabled())
      chassis.pid_tuner_disable();
  }
}

/**
 * If no competition control is connected, this function will run immediately
 * following initialize().
 */
void opcontrol() {
  // This is preference to what you like to drive on
  chassis.drive_brake_set(MOTOR_BRAKE_COAST);
  // track the state of the pneumatic pistons, false = retracted
  bool gateState = true;
  bool rodState = true;

  while (true) {
    // Gives you some extras to make EZ-Template ezier
    ez_template_extras();
    chassis.opcontrol_arcade_standard(ez::SPLIT);   // Standard split arcade

    // INTAKE CONTROLS: L1/2
    if(master.get_digital(DIGITAL_L1)){
      setIntake(127);
    }
    else if(master.get_digital(DIGITAL_L2)){
      setIntake(-127);
    }
    else{
      setIntake(0);
    }
    // CONVEYOR CONTROLS: R1/2
    if(master.get_digital(DIGITAL_R1)){
      setConveyor(127);
    }
    else if(master.get_digital(DIGITAL_R2)){
      setConveyor(-127);
    }
    else{
      setConveyor(0);
    }

    // --- Pneumatic Gate (Scooping from Matchloader) --- UP BUTTON
    if(master.get_digital_new_press(DIGITAL_UP)) {
      gateState = !gateState;
      pneumaticGate.set_value(gateState);
    }
    // --- Pneumatic Rod (Cleaving through scoring tubes) --- LEFT BUTTON
    if(master.get_digital_new_press(DIGITAL_LEFT)) {
      rodState = !rodState;
      pneumaticRod.set_value(rodState);
    }

    pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
