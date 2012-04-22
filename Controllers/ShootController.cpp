#include "ShootController.h"

ShootController2415::ShootController2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);

	turret = Task2415::SearchForTask("turret2415");
	intake = Task2415::SearchForTask("intake2415");
	
	//For practice, line sensor = 9
	//For real, line sensor = 7
	ballSensor = new DigitalInput(7);
	
	//For practice, (12,13...
	//For real, (10,11...
	towerEncoder = new Encoder(10,11,false,(CounterBase::EncodingType)2);
	
	taskState = WAIT_FOR_INPUT;

	Start("shootcontroller2415");
}

int ShootController2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool inIndexLoop;
	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();

			towerEncoder->Stop();
			towerEncoder->Reset();
			
			inIndexLoop = false;
		}
				
		if (taskStatus == STATUS_TELEOP) {
			if(!inIndexLoop){
				if(global->SecondaryGetRightBumper()){
	//				printf("GetValue: %g\n",turret->GetValue());
					if(turret->GetValue() >= global->ReadCSV("FLYWHEEL_LOWER_ERROR_MARGIN") 
					&& turret->GetValue() <= global->ReadCSV("FLYWHEEL_UPPER_ERROR_MARGIN")){
						intake->SetState(SHOOT);					
					} else {
						intake->SetState(WAIT_FOR_INPUT);
					}
				} else {
					intake->SetState(WAIT_FOR_INPUT);
				}
			}
						
			//Autoindexing:
			//Once light sensor triggered, run tower for set amount of encoder clicks
			//1 if it has nothing, 0 if it has a ball
			
//			printf("Tower: %d\n",towerEncoder->Get());
			
			if(!ballSensor->Get() && !inIndexLoop){
				inIndexLoop = true;
				towerEncoder->Start();
				intake->SetState(SHOOT);			
			}
			
			if(towerEncoder->Get() >= global->ReadCSV("CLICKS_INDEX")) {
				towerEncoder->Stop();
				towerEncoder->Reset();
				intake->SetState(WAIT_FOR_INPUT);
				inIndexLoop = false;
			}
			
			//If we're backdriving, then don't autoindex afterwards
			if(global->SecondaryGetButtonY()) {
				inIndexLoop = false;
			}
			
		}
		SwapAndWait();
	}
	return 0;
}
