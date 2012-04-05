#include "ShootController.h"

ShootController2415::ShootController2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);

	turret = Task2415::SearchForTask("turret2415");
	intake = Task2415::SearchForTask("intake2415");
	
	taskState = WAIT_FOR_INPUT;

	Start("shootcontroller2415");
}

int ShootController2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
		}
		
		if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			intake->SetState(WAIT_FOR_INPUT);
			if(global->SecondaryGetRightBumper()){
				printf("GetValue: %g\n",turret->GetValue());
				if(fabs(turret->GetValue()) <= ERROR_MARGIN){
					intake->SetState(SHOOT);					
				} 
			}
		}
		SwapAndWait();
	}
	return 0;
}
