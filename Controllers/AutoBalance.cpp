#include "AutoBalance.h"

AutoBalance2415::AutoBalance2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);

	drive = Task2415::SearchForTask("drive2415");
	
	stickDown = new Solenoid(2,8);
	stickUp = new Solenoid(2,7);
	
	taskState = WAIT_FOR_INPUT;

	Start("autobalance2415");
}

int AutoBalance2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			stickDown->Set(false);
			stickUp->Set(true);
		}
		
		if (taskStatus == STATUS_TELEOP) {
			if(global->PrimaryGetButtonBack()){
				stickDown->Set(true);
				stickUp->Set(false);
			} else {
				stickDown->Set(false);
				stickUp->Set(true);
			}
		}
		SwapAndWait();
	}
	return 0;
}
