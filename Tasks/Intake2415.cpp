#include "Intake2415.h"

Intake2415::Intake2415() {
	global = new Global();
	
	armUp = new Solenoid(4);
	armDown = new Solenoid(3);
	
	backplateDisengage = new Solenoid(6);
	backplateEngage = new Solenoid(5);
	
	suction = new Relay(2);
	
	oneBallShoot = new Timer();
	twoBallShoot = new Timer();
	
	taskState = WAIT_FOR_INPUT;

	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	
	bool prevXState = true;
	bool isArmUp = false;
	bool prevTrigState = false;
	bool prevBumpState = false;
	
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			oneBallShoot->Stop();
			oneBallShoot->Reset();
			twoBallShoot->Stop();
			twoBallShoot->Reset();
			
			prevXState = true;
			isArmUp = false;
			prevTrigState = false;
			prevBumpState = false;
		}
		
		if(taskStatus == STATUS_AUTO){
			switch(taskState) {
			case WAIT_FOR_INPUT:
				if(twoBallShoot->Get() >= global->ReadCSV("TWO_BALL_SHOOT")) {
					backplateEngage->Set(false);
					backplateDisengage->Set(true);
					twoBallShoot->Stop();
					twoBallShoot->Reset();
				}
				break;
			case AUTONOMOUS_SHOOT:			
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
				twoBallShoot->Start();
				taskState = WAIT_FOR_INPUT;
				break;
			default:
				break;
			}
		}
		
		if (taskStatus == STATUS_TELEOP) {
			if(global->PrimaryGetButtonY()) {
				suction->Set(suction->kReverse); //I don't think this actually will work, we'll see
			} else {
				suction->Set(suction->kForward); //I should be able to just say kReverse...
			}
			
			//Two Ball
			if(global->SecondaryGetLeftTrigger() && !prevTrigState) {
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
				twoBallShoot->Start();
			} 
			
			if(twoBallShoot->Get() >= global->ReadCSV("TWO_BALL_SHOOT")) {
				backplateEngage->Set(false);
				backplateDisengage->Set(true);
				twoBallShoot->Stop();
				twoBallShoot->Reset();
			}
			
			//One ball
			if(global->SecondaryGetLeftBumper() && !prevBumpState) { 
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
				oneBallShoot->Start();
			} 
			
			if(oneBallShoot->Get() >= global->ReadCSV("ONE_BALL_SHOOT")) {
				backplateEngage->Set(false);
				backplateDisengage->Set(true);
				oneBallShoot->Stop();
				oneBallShoot->Reset();
			}
			
			if(global->PrimaryGetButtonX() && !prevXState) { //Toggle structure
				if(isArmUp) {
					isArmUp = false;
					armUp->Set(false);
					armDown->Set(true);
				} else {
					isArmUp = true;
					armUp->Set(true);
					armDown->Set(false);
				}
			}			
			prevXState = global->PrimaryGetButtonX();
			prevTrigState = global->SecondaryGetLeftTrigger();
			prevBumpState = global->SecondaryGetLeftBumper();
		}
		SwapAndWait();
	}

	return 0;
}
