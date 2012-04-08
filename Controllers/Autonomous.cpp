#include "Autonomous.h"

Autonomous2415::Autonomous2415(void) {
	global = new Global();

	intake = Task2415::SearchForTask("intake2415");
	turret = Task2415::SearchForTask("turret2415");
	waitTimer = new Timer();
	totalTimer = new Timer();
		
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
			totalTimer->Stop();
			totalTimer->Reset();
		}
		
		if (taskStatus == STATUS_AUTO) {
			switch(taskState){
			case START:
				waitTimer->Start();
				totalTimer->Start();
				taskState = WAIT;
				intake->SetState(WAIT_FOR_AUTO_INPUT);
				turret->SetState(AUTO_FIRST_BALLS);
				break;
			case WAIT:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME")){
					waitTimer->Stop();
					waitTimer->Reset();
					taskState = SHOOT;
				}
				break;
			case SHOOT:
				intake->SetState(SHOOT);
				waitTimer->Start();
				turret->SetState(AUTO_REST_BALLS);
				taskState = NEW_TURRET;
				break;
			case NEW_TURRET:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME")){
					waitTimer->Stop();
					waitTimer->Reset();
					intake->SetState(SECOND_SHOOT);
					taskState = LAST_TURRET;
				}
				break;
			case LAST_TURRET:
				if(totalTimer->Get() >= global->ReadCSV("TOTAL_AUTO_WAIT")){
					turret->SetState(AUTO_FINAL_BALLS);
					totalTimer->Stop();
					totalTimer->Reset();
					taskState = END;
				}
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

