#include "Drive2415.h"

Drive2415::Drive2415(void) {
	global = new Global();

	vicLeft = new Victor(1);
	vicRight = new Victor(2);

	//taskState = NORMAL_JOYSTICK;

	Start("drive2415");
}

int Drive2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	while (keepTaskAlive) { //Deleted all "negative inertia" because unnecessary and causing weird backwards driving
		if (taskStatus == STATUS_AUTO || taskStatus == STATUS_TELEOP) {
			double throttle = -global->GetLeftY();
			double wheel = global->GetRightX();
			
			bool isQuickTurn = global->GetLeftTrigger1(); //Get some button (hold-down, not toggle)
			bool isHighGear = global->GetButtonA(); //Get some button (hold-up)
			
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
				angular_power = -wheel;
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
			vicRight->Set(global->LinearizeVictor(right_pwm));
		}

		SwapAndWait();
	}

	return 0;
}

