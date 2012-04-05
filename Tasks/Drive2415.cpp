#include "Drive2415.h"

Drive2415::Drive2415() {
	global = new Global;

	vicLeft = new Victor(1);
	vicRight = new Victor(2);
	
	brakeOff = new Solenoid(8);
	brakeNow = new Solenoid(7);
	
	stupidTimer = new Timer();
	
	//taskState = NORMAL_JOYSTICK;

	Start("drive2415");
}

//vicLeft->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));
//vicRight->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));

int Drive2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool isBraked, prevTrigState;
	bool firstTime = true;
	
	while (keepTaskAlive) { 		
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			isBraked = false;
			prevTrigState = false;			
			stupidTimer->Stop();
			stupidTimer->Reset();
			firstTime = true;
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO) {
			switch(taskState){
			case FORWARD:
				vicLeft->Set(STRAIGHT_SPEED);
				vicRight->Set(-STRAIGHT_SPEED);
				break;
			case BACK:
				vicLeft->Set(-STRAIGHT_SPEED);
				vicRight->Set(STRAIGHT_SPEED);
				break;
			case TURN_FORWARD_RIGHT:
				vicLeft->Set(TURN_SPEED);
				vicRight->Set(-TURN_SPEED - 0.15);
				break;
			case TURN_FORWARD_LEFT:
				vicLeft->Set(TURN_SPEED + 0.15);
				vicRight->Set(-TURN_SPEED);
				break;
			case TURN_BACKWARD_RIGHT:
				vicLeft->Set(-STRAIGHT_SPEED);
				vicRight->Set(STRAIGHT_SPEED + 0.15);
				break;
			case TURN_BACKWARD_LEFT:
				vicLeft->Set(-STRAIGHT_SPEED - 0.15);
				vicRight->Set(STRAIGHT_SPEED);
				break;
			default:
				vicLeft->Set(0.0);
				vicRight->Set(0.0);
				break;
			}
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////		
		if (taskStatus == STATUS_TELEOP) {			
			if(firstTime){
				stupidTimer->Start();
				firstTime = false;
			}
			
			double left = -global->PrimaryGetLeftY();
			double right = global->PrimaryGetRightY();
			
			if(taskState == AUTOBALANCE) {
				left *= global->ReadCSV("BALANCE_SLOW_DRIVE");
				right *= global->ReadCSV("BALANCE_SLOW_DRIVE");
			}
			
			//Toggle the brakes
			if(global->PrimaryGetLeftTrigger() && !prevTrigState && stupidTimer->Get() >= 90) {
				stupidTimer->Stop();
				if(isBraked) {
					isBraked = false;
					brakeOff->Set(true);
					brakeNow->Set(false);
				} else {
					isBraked = true;
					brakeOff->Set(false);
					brakeNow->Set(true);
				}
			}
			prevTrigState = global->PrimaryGetLeftTrigger();
			
			if(!isBraked){
				vicLeft->Set(global->LinearizeVictor(left));
				vicRight->Set(global->LinearizeVictor(right));
			}
		}
		SwapAndWait();
	}

	return 0;
}

