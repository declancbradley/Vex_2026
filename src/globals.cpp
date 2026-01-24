#include "main.h"
// defining the actual things announced in the globals.hpp file

//motors
pros::Motor intake(8, pros::v5::MotorGears::green);
pros::Motor conveyor(10, pros::v5::MotorGears::green);
pros::MotorGroup intakeAndConveyor({8, 10}, pros::v5::MotorGears::green);