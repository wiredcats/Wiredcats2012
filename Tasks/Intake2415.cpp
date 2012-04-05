#include "Intake2415.h"

Intake2415::Intake2415() {
	global = new Global();
	
	armUp = new Solenoid(3);
	armDown = new Solenoid(4);
	
	towerSuction = new Relay(2);
	feed = new Relay(3);
	
	ballSensor = new DigitalInput(9);
	towerEncoder = new Encoder(12,13,false,(CounterBase::EncodingType)2);
	
	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	
	bool prevOtherXState, prevTrigState, prevBumpState;
	
	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
						
			prevTrigState = false;
			prevBumpState = false;
			prevOtherXState = false;
			towerEncoder->Stop();
			towerEncoder->Reset();
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO){
			switch(taskState) {
			case WAIT_FOR_AUTO_INPUT:
				towerSuction->Set(towerSuction->kOff);
				break;
			case SHOOT:			
				towerSuction->Set(towerSuction->kReverse);
				break;
			default:
				break;
			}
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////		
		if (taskStatus == STATUS_TELEOP) {	
			// Arm control //
			if(global->PrimaryGetLeftTrigger()) {
				armUp->Set(false);
				armDown->Set(true);
			} else {
				armUp->Set(true);
				armDown->Set(false);
			}	
			
			if(global->PrimaryGetLeftBumper()){
				feed->Set(feed->kReverse);
			} else {
				feed->Set(feed->kOff);
			}
			
//			printf("Tower Encoder: %d\n",towerEncoder->Get());			
			
			//Autoindexing:
			//Once light sensor triggered, run tower for set amount of encoder clicks
			//1 if it has nothing, 0 if it has a ball
			if(!ballSensor->Get()){
				towerEncoder->Start();
				towerSuction->Set(towerSuction->kReverse);				
			} else {
				towerSuction->Set(towerSuction->kOff);
			}
			
			if(towerEncoder->Get() >= CLICKS_INDEX) {
				towerEncoder->Stop();
				towerEncoder->Reset();
				towerSuction->Set(towerSuction->kOff);
			}
			
			if(taskState == SHOOT){
				towerSuction->Set(towerSuction->kReverse);
			}
			
			// Backdrive the tower //
			if(global->SecondaryGetButtonY()) {
				towerSuction->Set(towerSuction->kForward); 
			} 
			
			prevOtherXState = global->SecondaryGetButtonX();
			prevTrigState = global->SecondaryGetLeftTrigger();
			prevBumpState = global->SecondaryGetLeftBumper();
		}
		SwapAndWait();
	}

	return 0;
}
