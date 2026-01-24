#include "main.h"
// define helper functions that are announced in helpers.hpp

// 127 is on, 0 is off
void setIntake(int intakePower){
    intake.move(intakePower);
}
void setConveyor(int conveyorPower){
    conveyor.move(conveyorPower);
}
void setIntakeAndConveyor(int combinedPower){
    intakeAndConveyor.move(combinedPower);
}