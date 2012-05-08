#include "Intake2415.h"

//Up: A and D are on
//
//To do bridge: Switch Off A and D, turn on C. Turn on B for set amount of time and then turn off. Turn A back on
//
//Ball: A and C are on. D turns off. 
//
//A can not be on while B is on. C can not be on while D is on. Out of these pairs, one must always be on.
//
//Arm down is C
//arm Up is D
//Bridge up is A
//Bridge down is B

enum ARM_STATUS {
	ALL_UP, BALL_GRAB, BRIDGE_PUSH
};

Intake2415::Intake2415() {
	global = new Global();
	
	solD = new Solenoid(1,5);
	solC = new Solenoid(1,6);
	
	solA = new Solenoid(1,4);
	solB = new Solenoid(1,3);
	
	feed = new Victor(5);
	tower = new Relay(8);
	
	pulseTimer = new Timer();
		
	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	
	bool prevOtherXState;
	bool hasPulsed;
	ARM_STATUS armState = ALL_UP;
	
	while (keepTaskAlive) {
		
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			tower->Set(tower->kOff);			
			prevOtherXState = false;
			armState = ALL_UP;
			pulseTimer->Stop();
			pulseTimer->Reset();
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
				armState = BALL_GRAB;
				feed->Set(-1.0);
				break;
			case SECOND_SHOOT:
				armState = BALL_GRAB;
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
			
			//Arm Control
			if(global->PrimaryGetLeftBumper()){
				armState = BALL_GRAB;
			} else{
				armState = ALL_UP;
				if(global->PrimaryGetRightBumper()){
					armState = BRIDGE_PUSH;
				}
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
		
		switch(armState){
		case ALL_UP:
			solA->Set(true);
			solD->Set(true);
			solB->Set(false);
			solC->Set(false);
			hasPulsed = false;
			pulseTimer->Stop();
			pulseTimer->Reset();
			break;
		case BRIDGE_PUSH:
			solB->Set(true);
			solC->Set(true);
			solD->Set(false);
			solA->Set(false);
			hasPulsed = false;
			pulseTimer->Stop();
			pulseTimer->Reset();
			break;
		case BALL_GRAB:
			if(!hasPulsed){
				hasPulsed = true;
				solA->Set(false);
				solD->Set(false);
				solC->Set(true);
				pulseTimer->Start();
				solB->Set(true);
			}
			if(hasPulsed && pulseTimer->Get() >= global->ReadCSV("PULSE_TIME")){
				pulseTimer->Stop();
				solB->Set(false);
				solA->Set(true);				
			}			
			break;
		default:
			armState = ALL_UP;
			break;
		}
		
		SwapAndWait();
	}

	return 0;
}
