#include "Autonomous.h"

Autonomous::Autonomous(void) {
	global = new Global();
	
	drive = Task2415::SearchForTask("drive2415");
	intake = Task2415::SearchForTask("intake2415");
	
	waitTimer = new Timer();
	
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
			waitTimer->Stop();
			waitTimer->Reset();
		}		
		if (taskStatus == STATUS_AUTO) {
			//Wait for a bit and then shoot with key shot
			switch(taskState) {
			case START:
				waitTimer->Start();
				taskState = WAIT_FORWARD;
				break;
			case WAIT_FORWARD:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME") ) {
					taskState = SHOOT;
					waitTimer->Stop();
					waitTimer->Reset();
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

