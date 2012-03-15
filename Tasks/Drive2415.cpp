#include "Drive2415.h"

//	bool prevTrigState = true;
//	bool isHighGear = false;
	
	//			if(global->GetRightTrigger1() && !prevTrigState) { //Toggle structure
	//				if(isHighGear) {
	//					isHighGear = false;
	//					highGear->Set(false);
	//					lowGear->Set(true);
	//				} else {
	//					isHighGear = true;
	//					highGear->Set(true);
	//					lowGear->Set(false);
	//				}
	//			}			
	//			prevTrigState = global->GetRightTrigger1();

Drive2415::Drive2415() {
	global = new Global;

	vicLeft = new Victor(1);
	vicRight = new Victor(2);
	
	lowGear = new Solenoid(7);
	highGear = new Solenoid(8);

	//taskState = NORMAL_JOYSTICK;

	Start("drive2415");
}

int Drive2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	while (keepTaskAlive) { //Deleted all "negative inertia" because unnecessary and causing weird backwards driving
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
		}
		
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
		
		if (taskStatus == STATUS_TELEOP) {			
			double throttle = -global->PrimaryGetLeftY();
			double wheel = global->PrimaryGetRightX();
			
			if(taskState == AUTOBALANCE) {
				throttle *= global->ReadCSV("BALANCE_SLOW_DRIVE");
			}
						
			bool isQuickTurn = global->PrimaryGetLeftBumper(); 
			bool isHighGear = global->PrimaryGetLeftTrigger();
			
			if(isHighGear){
				highGear->Set(false);
				lowGear->Set(true);
			} else {
				highGear->Set(true);
				lowGear->Set(false);
			}
			
			float linear_power = throttle;
			
			double wheelNonLinearity;
			
			if(isHighGear) {
				wheelNonLinearity = global->ReadCSV("TURN_NONLIN_HIGH");
				} else {
				wheelNonLinearity = global->ReadCSV("TURN_NONLIN_LOW");
				wheel = sin(PI / 2.0 * wheelNonLinearity * wheel) / sin(PI / 2.0 * wheelNonLinearity);
			}
			
			wheel = sin(PI / 2.0 * wheelNonLinearity * wheel) / sin(PI / 2.0 * wheelNonLinearity);
			wheel = sin(PI / 2.0 * wheelNonLinearity * wheel) / sin(PI / 2.0 * wheelNonLinearity);
			
			double left_pwm, right_pwm, overPower;
			float sensitivity = 1.7;
			
			float angular_power;
			
			if (isHighGear) {
				sensitivity = global->ReadCSV("SENSE_HIGH");
			} else {
				sensitivity = global->ReadCSV("SENSE_LOW");
				
				if (fabs(throttle) > global->ReadCSV("SENSE_CUTTOFF")) {
					sensitivity = 1 - (1 - sensitivity) / fabs(throttle);
				}
			}
			
			//quickturn!
			if (isQuickTurn) {
				overPower = 1.0;
				sensitivity = 1.0;
				angular_power = wheel;
			} else {
				overPower = 0.0;
				angular_power = throttle * wheel * sensitivity; //Unabsolute valued this so that the turns wouldn't be backwards
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
			
			vicLeft->Set(global->LinearizeVictor(left_pwm));
			vicRight->Set(global->LinearizeVictor(-right_pwm));
		}

		SwapAndWait();
	}

	return 0;
}

