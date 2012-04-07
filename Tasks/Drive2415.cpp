#include "Drive2415.h"

Drive2415::Drive2415() {
	global = new Global;

	vicLeft = new Victor(1);
	vicRight = new Victor(2);
	
	brakeOff = new Solenoid(3,8);
	brakeNow = new Solenoid(3,7);
	
	stupidTimer = new Timer();
	
	//taskState = NORMAL_JOYSTICK;

	Start("drive2415");
}

//vicLeft->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));
//vicRight->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));

int Drive2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool isBraked, prevBState;
	bool firstTime = true;
	
	while (keepTaskAlive) { 		
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			isBraked = false;
			prevBState = false;			
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
				vicLeft->Set(global->ReadCSV("STRAIGHT_SPEED"));
				vicRight->Set(-global->ReadCSV("STRAIGHT_SPEED"));
				break;
			case BACK:
				vicLeft->Set(-global->ReadCSV("STRAIGHT_SPEED"));
				vicRight->Set(global->ReadCSV("STRAIGHT_SPEED"));
				break;
			case TURN_FORWARD_RIGHT:
				vicLeft->Set(global->ReadCSV("TURN_SPEED"));
				vicRight->Set(-global->ReadCSV("TURN_SPEED") - 0.15);
				break;
			case TURN_FORWARD_LEFT:
				vicLeft->Set(global->ReadCSV("TURN_SPEED") + 0.15);
				vicRight->Set(-global->ReadCSV("TURN_SPEED"));
				break;
			case TURN_BACKWARD_RIGHT:
				vicLeft->Set(-global->ReadCSV("STRAIGHT_SPEED"));
				vicRight->Set(global->ReadCSV("STRAIGHT_SPEED") + 0.15);
				break;
			case TURN_BACKWARD_LEFT:
				vicLeft->Set(-global->ReadCSV("STRAIGHT_SPEED") - 0.15);
				vicRight->Set(global->ReadCSV("STRAIGHT_SPEED"));
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
			
			if(global->PrimaryGetRightBumper()) {
				left *= global->ReadCSV("BALANCE_SLOW_DRIVE");
				right *= global->ReadCSV("BALANCE_SLOW_DRIVE");
			}
			
			//Cheesy poofs stinks so we're writing our own "add a constant" deadband
			left *= 0.885;
			right *= 0.92;
			
			left += 0.115 * Sign(left);
			right += 0.08 * Sign(right);
			
			//Toggle the brakes
			if(global->PrimaryGetButtonB() && !prevBState && stupidTimer->Get() >= 90) {
				stupidTimer->Stop();
				if(isBraked) {
					isBraked = false;
				} else {
					isBraked = true;
				}
			}
			prevBState = global->PrimaryGetButtonB();
			
			if(isBraked){
				brakeOff->Set(false);
				brakeNow->Set(true);
			} else {
				brakeOff->Set(true);
				brakeNow->Set(false);
			}
			
			if(!isBraked){
//				vicLeft->Set(global->LinearizeVictor(left));
//				vicRight->Set(global->LinearizeVictor(right));
				vicLeft->Set(left);
				vicRight->Set(right);
			}
		}
		SwapAndWait();
	}

	return 0;
}

int Drive2415::Sign(float val){
	if(val >= 0) {
		return 1;
	}
	if(val < 0) {
		return -1;
	}
	return(1);
}


