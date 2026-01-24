#include "main.h"
// defining the actual things announced in the globals.hpp file

//motors
pros::Motor intake(3, pros::v5::MotorGears::green);
pros::Motor conveyor(8, pros::v5::MotorGears::green);
pros::MotorGroup intakeAndConveyor({3, 8}, pros::v5::MotorGears::green);

//pnuematics
// gate lowered when stealing from matchloader, rod only used for scoring in driver control
pros::adi::Pneumatics pneumaticGate('H', false); // port, position (false = retraction = gate up)
pros::adi::Pneumatics pneumaticRod('G', false); // port, position (false = retraction = rod down)