#include "ShootController.h"

Shoot2415::Shoot2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);

	intake = Task2415::SearchForTask("intake2415");
	turret = Task2415::SearchForTask("turret2415");
	
	taskState = WAIT_FOR_INPUT;

	Start("shoot2415");
}

int Shoot2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////
		if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			switch(taskState){
			case WAIT_FOR_INPUT:
				break;
			default:
				break;
			}
		}
		SwapAndWait();
	}
	return 0;
}
