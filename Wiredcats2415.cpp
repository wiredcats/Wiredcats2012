#include "Wiredcats2415.h"

Task2415 *Task2415::TaskList[TASK2415_MAX_TASKS + 1] = {NULL};
int Task2415::TasksListed = 0;
int Task2415::TasksSwapped = 0;
bool Task2415::FirstConstructor = true;

Global global;

Wiredcats2415::Wiredcats2415(void) {
	compressor = new Compressor(6,1);	
}

void Wiredcats2415::Disabled(void) {
	Task2415::SetTaskStatuses(STATUS_DISABLED);
	global.ResetCSV();
}

void Wiredcats2415::Autonomous(void) {
	Task2415::SetTaskStatuses(STATUS_AUTO);	
	compressor->Start();
}

void Wiredcats2415::OperatorControl(void) {
	Task2415::SetTaskStatuses(STATUS_TELEOP);
	compressor->Start();
}

START_ROBOT_CLASS(Wiredcats2415);

