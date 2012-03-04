#include "Intake2415.h"

Intake2415::Intake2415() {
	global = new Global();
	
	armUp = new Solenoid(4);
	armDown = new Solenoid(3);
	
	backplateDisengage = new Solenoid(6);
	backplateEngage = new Solenoid(5);
	
	suction = new Relay(2);

	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	while (keepTaskAlive) {
		if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			if(global->GetButtonB()) {
				suction->Set(suction->kOff); //I don't think this actually will work, we'll see
			} else {
				suction->Set(suction->kReverse); //I should be able to just say kReverse...
			}
			
			if(global->GetRightTrigger2()) {
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
			} else {
				backplateEngage->Set(false);
				backplateDisengage->Set(true);
			}
			
			if(global->GetLeftTrigger2()) {
				armUp->Set(false);
				armDown->Set(true);
			} else {
				armUp->Set(true);
				armDown->Set(false);
			}
		}
		SwapAndWait();
	}

	return 0;
}
