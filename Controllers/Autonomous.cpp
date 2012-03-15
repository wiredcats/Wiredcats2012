#include "Autonomous.h"

Autonomous::Autonomous(void) {
	global = new Global();
	
	drive = Task2415::SearchForTask("drive2415");
	intake = Task2415::SearchForTask("intake2415");
	
	driveTimer = new Timer();
	
	printf("stalling to allow tasks to be initialized\n");
	Wait(1.0);
	
	taskState = START;

	Start("auto2415");
}

int Autonomous::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED){
			global->ResetCSV();
			driveTimer->Stop();
			driveTimer->Reset();
		}		
		if (taskStatus == STATUS_AUTO) {
			//Drive until right time value == right up against fender
			//Shoot with fender shot
			
			switch(taskState) {
			case START:
				drive->SetState(FORWARD);
				driveTimer->Start();
				taskState = DRIVE_FORWARD;
				break;
			case DRIVE_FORWARD:
				if(driveTimer->Get() >= global->ReadCSV("AUTONOMOUS_DRIVE_TIME") ) {
					taskState = SHOOT;
					driveTimer->Stop();
					driveTimer->Reset();
					drive->SetState(NORMAL_JOYSTICK);
				}
				break;
			case SHOOT:
				intake->SetState(AUTONOMOUS_SHOOT);
				taskState = END;
				break;
			case END:
				break;
			default:				
				break;
			}
		}
		SwapAndWait();
	}

	return 0;
}

