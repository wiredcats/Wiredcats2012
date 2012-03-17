#include "Drive2415.h"

Drive2415::Drive2415() {
	global = new Global;

	vicLeft = new Victor(1);
	vicRight = new Victor(2);
	
	brakeOff = new Solenoid(8);
	brakeNow = new Solenoid(7);

	//taskState = NORMAL_JOYSTICK;

	Start("drive2415");
}

int Drive2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool isBraked, prevTrigState;
	
	while (keepTaskAlive) { 
		
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			isBraked = false;
			prevTrigState = false;			
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO) {
			switch(taskState){
			case FORWARD:
				vicLeft->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));
				vicRight->Set(global->ReadCSV("AUTONOMOUS_DRIVE_SPEED"));
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
			//TODO: NEED TO TUNE THESE BETTER
			double throttle = -global->PrimaryGetLeftY();
			double wheel = global->PrimaryGetRightX();
			
			if(taskState == AUTOBALANCE) {
				throttle *= global->ReadCSV("BALANCE_SLOW_DRIVE");
			}
						
			bool isQuickTurn = global->PrimaryGetLeftBumper(); 
			
			//Toggle the brakes
			if(global->PrimaryGetLeftTrigger() && !prevTrigState ) {
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
						
			float linear_power = throttle;
			
			double wheelNonLinearity;
			
			wheelNonLinearity = global->ReadCSV("TURN_NONLIN");
			wheel = sin(PI / 2.0 * wheelNonLinearity * wheel) / sin(PI / 2.0 * wheelNonLinearity);	
//			wheel = sin(PI / 2.0 * wheelNonLinearity * wheel) / sin(PI / 2.0 * wheelNonLinearity);
			
			double left_pwm, right_pwm, overPower;
			float sensitivity = sensitivity = global->ReadCSV("SENSE_LOW");
			
			if (fabs(throttle) > global->ReadCSV("SENSE_CUTTOFF")) {
				sensitivity = 1 - (1 - sensitivity) / fabs(throttle);
			}
			
			float angular_power;
				
			//quickturn!
			if (isQuickTurn) {
				overPower = 1.0;
				sensitivity = 1.0;
				angular_power = wheel;
			} else {
				overPower = 0.0;
				angular_power = throttle * wheel * sensitivity; 
			}
			
			left_pwm = linear_power + angular_power;
			right_pwm = linear_power - angular_power;
			
			if (left_pwm > 1.0) {
				right_pwm -= overPower*(left_pwm - 1.0);
				left_pwm = 1.0;
				} else if (right_pwm > 1.0) {
					left_pwm -= overPower*(right_pwm - 1.0);
					right_pwm = 1.0;
					} else if (left_pwm < -1.0) {
						right_pwm += overPower*(-1.0 - left_pwm);
						left_pwm = -1.0;
						} else if (right_pwm < -1.0) {
							left_pwm += overPower*(-1.0 - right_pwm);
							right_pwm = -1.0;
						}
			if(!isBraked){
				vicLeft->Set(global->LinearizeVictor(left_pwm));
				vicRight->Set(global->LinearizeVictor(-right_pwm));
			}
		}
		SwapAndWait();
	}

	return 0;
}

