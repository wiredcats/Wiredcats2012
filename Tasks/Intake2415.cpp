#include "Intake2415.h"

Intake2415::Intake2415(void) {
	global = new Global();
	
	armUp = new Solenoid(1);
	armDown = new Solenoid(2);
	
	backplateEngage = new Solenoid(4);
	backplateDisengage = new Solenoid(3);
	
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
				printf("right Trigger 2 hit\n");
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
			} else {
				backplateEngage->Set(false);
				backplateDisengage->Set(true);
			}
			
			if(global->GetLeftTrigger2()) {
				printf("left trigger 2 hit\n");
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
