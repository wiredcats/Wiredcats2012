#include "AutoBalance.h"

AutoBalance2415::AutoBalance2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);

	drive = Task2415::SearchForTask("drive2415");
	
	gyro = new Gyro(1,1);
	
	backdriveTimer = new Timer();
	stopTimer = new Timer();
	
	taskState = WAIT_FOR_INPUT;

	Start("autobalance2415");
}

int AutoBalance2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	gyro->Reset();
	int direction;
	
	while (keepTaskAlive) {
		if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			float angle = gyro->GetAngle();			
			switch (taskState) {
				case WAIT_FOR_INPUT:
					drive->SetState(NORMAL_JOYSTICK); //Should make the panic button toggle
					if (global->GetRightTrigger1() && !CheckIfBalanced(angle)) {
						taskState = BALANCE_LOOP;
					}
					break;
				case BALANCE_LOOP:
					if( CheckIfBalanced(angle)) {
						taskState = BACKDRIVE;
					} else {
						if (angle >= BALANCE_MARGIN_OF_ERROR) {
							drive->SetState(MOVE_BACK);
							direction = -1;
							printf("moving on back\n");
						} else {
							if (angle <= -BALANCE_MARGIN_OF_ERROR) {
								drive->SetState(GO_STRAIGHT);
								printf("going forward\n");
								direction = 1;
							}
						}
					}
					break;
				case BACKDRIVE:
					if (direction == 1) {
						drive->SetState(MOVE_BACKDRIVE_BACK);
						backdriveTimer->Start();
						direction = 0;
					}
					if (direction == -1) {
						drive->SetState(GO_BACKDRIVE_STRAIGHT);
						backdriveTimer->Start();
						direction = 0;
					}
					if (backdriveTimer->Get() >= BACKDRIVE_PERIOD) {
						backdriveTimer->Stop();
						backdriveTimer->Reset();
						taskState = STOP;
						stopTimer->Reset();
						stopTimer->Start();
					}
					break;
				case STOP:
					drive->SetState(STOP_BOT);
					if(stopTimer->Get() >= STOP_PERIOD || !(global->GetRightTrigger1())) {
						stopTimer->Stop();
						stopTimer->Reset();
						taskState = FINAL_CHECK;
					}
					break;
				case FINAL_CHECK:
					if( CheckIfBalanced(angle) || !(global->GetRightTrigger1())) {
						taskState = WAIT_FOR_INPUT;						
					} else {
						taskState = BALANCE_LOOP;
					}
					break;
				default:
					taskState = WAIT_FOR_INPUT;
					break;
			}
		}
		SwapAndWait();
	}
	return 0;
}

bool AutoBalance2415::CheckIfBalanced(float angle) {
	return ( -BALANCE_MARGIN_OF_ERROR <= angle && angle <= BALANCE_MARGIN_OF_ERROR);
}
