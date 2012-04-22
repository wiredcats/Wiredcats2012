#include "Intake2415.h"

Intake2415::Intake2415() {
	global = new Global();
	
	armUp = new Solenoid(1,5);
	armDown = new Solenoid(1,6);
	
	bridgeUp = new Solenoid(1,3);
	bridgeDown = new Solenoid(1,4);
	
	feed = new Victor(5);
	tower = new Relay(2);
		
	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	
	bool prevOtherXState;
	bool isBridgeUp, isArmUp;
	
	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			tower->Set(tower->kOff);			
			prevOtherXState = false;
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO){
			switch(taskState) {
			case WAIT_FOR_AUTO_INPUT:
//				printf("Waiting...\n");
				tower->Set(tower->kOff);
				break;
			case SHOOT:			
//				printf("We are shooting\n");
				tower->Set(tower->kReverse);
				break;
			case SECOND_SHOOT:
				armUp->Set(false);
				armDown->Set(true);
				feed->Set(-1.0);
				break;
			default:
				break;
			}
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////		
		if (taskStatus == STATUS_TELEOP) {				
			if(taskState == SHOOT){
				tower->Set(tower->kReverse);
			}
			
			if(taskState == WAIT_FOR_INPUT){
				tower->Set(tower->kOff);
			}
			
			// Arm control //						
			if(global->PrimaryGetLeftTrigger()) {
				isArmUp = false;
				isBridgeUp = true;
			} else {
				isArmUp = true;
			}
			
			if(global->PrimaryGetLeftBumper()){
				isBridgeUp = false;
				isArmUp = true;
			} else {
				isBridgeUp = true;
			}
			
			if(isArmUp){
				armUp->Set(true);
				armDown->Set(false);
			} else {
				armUp->Set(false);
				armDown->Set(true);
			}
			
			if(isBridgeUp){
				bridgeUp->Set(true);
				bridgeDown->Set(false);
			} else {
				bridgeUp->Set(false);
				bridgeDown->Set(true);
			}
						
			if(global->SecondaryGetButtonB()){
				feed->Set(-1.0);
			} else {
				feed->Set(0.0);
			}
						
			// Backdrive the tower //
			if(global->SecondaryGetButtonY()) {
				tower->Set(tower->kForward); 
				feed->Set(1.0);
			} 
			
			prevOtherXState = global->SecondaryGetButtonX();
		}
		SwapAndWait();
	}

	return 0;
}
