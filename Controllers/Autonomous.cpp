#include "Autonomous.h"

Autonomous2415::Autonomous2415(void) {
	global = new Global();
	
	turret = Task2415::SearchForTask("turret2415");
	intake = Task2415::SearchForTask("intake2415");
	
	waitTimer = new Timer();
	
	printf("stalling to allow tasks to be initialized\n");
	Wait(1.0);
	
	taskState = START;

	Start("auto2415");
}

int Autonomous2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED){
			global->ResetCSV();
			turret->SetState(AUTO_FIRE);
			intake->SetState(WAIT_FOR_AUTO_INPUT);
			waitTimer->Stop();
			waitTimer->Reset();
		}		
		if (taskStatus == STATUS_AUTO) {
			//Wait for a bit and then shoot with key shot
			switch(taskState) {
			case START:
				waitTimer->Start();
				turret->SetState(AUTO_FIRE);
				taskState = WAIT;
				break;
			case WAIT:
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

