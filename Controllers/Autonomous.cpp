#include "Autonomous.h"

Autonomous2415::Autonomous2415(void) {
	global = new Global();

	intake = Task2415::SearchForTask("intake2415");
	waitTimer = new Timer();
		
	taskState = START;	
	Start("autonomous2415");
}

int Autonomous2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			taskState = START;
			waitTimer->Stop();
			waitTimer->Reset();
		}
		
		if (taskStatus == STATUS_AUTO) {
			switch(taskState){
			case START:
				waitTimer->Start();
				taskState = WAIT;
				intake->SetState(WAIT_FOR_AUTO_INPUT);
				break;
			case WAIT:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME")){
					intake->SetState(SHOOT);
					waitTimer->Stop();
					waitTimer->Reset();
					taskState = SHOOT;
				}
				break;
			case SHOOT:
				intake->SetState(SHOOT);
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

