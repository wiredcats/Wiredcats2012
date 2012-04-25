#include "Drive2415.h"

Drive2415::Drive2415() {
	global = new Global;

	vicLeft = new Victor(1);
	vicRight = new Victor(2);
	
	brakeOff = new Solenoid(1,8);
	brakeNow = new Solenoid(1,7);
	
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
				vicRight->Set(-global->ReadCSV("TURN_SPEED") - global->ReadCSV("TURN_GAIN"));
				break;
			case TURN_FORWARD_LEFT:
				vicLeft->Set(global->ReadCSV("TURN_SPEED") + global->ReadCSV("TURN_GAIN"));
				vicRight->Set(-global->ReadCSV("TURN_SPEED"));
				break;
			case TURN_BACKWARD_RIGHT:
				vicLeft->Set(-global->ReadCSV("TURN_SPEED"));
				vicRight->Set(global->ReadCSV("TURN_SPEED") + global->ReadCSV("TURN_GAIN"));
				break;
			case TURN_BACKWARD_LEFT:
				vicLeft->Set(-global->ReadCSV("TURN_SPEED") - global->ReadCSV("TURN_GAIN"));
				vicRight->Set(global->ReadCSV("TURN_SPEED"));
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
			
			//Using the family of equaztions in Ether's whitepaper
			double a = global->ReadCSV("DRIVE_GAIN");
			double b = global->ReadCSV("DRIVE_DEADBAND");
			
			if(left >= 0){
				left = b + (1-b)*(a*pow(left,3) + (1-a)*left);
			} else {
				left = -b + (1-b)*(a*pow(left,3) + (1-a)*left);
			}
			
			if(right >= 0){
				right = b + (1-b)*(a*pow(right,3) + (1-a)*right);
			} else {
				right = -b + (1-b)*(a*pow(right,3) + (1-a)*right);
			}
			
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


