#include "Intake2415.h"

Intake2415::Intake2415() {
	global = new Global();
	
	armUp = new Solenoid(3);
	armDown = new Solenoid(4);
	
	backplateDisengage = new Solenoid(6);
	backplateEngage = new Solenoid(5);
	
	suction = new Relay(2);
	
	oneBallShoot = new Timer();
	twoBallShoot = new Timer();
	
	taskState = DISENGAGE;

	Start("intake2415");
}

int Intake2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	
	bool prevOtherXState, prevTrigState, prevBumpState;
	bool intakeOn, willShoot;
	
	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			oneBallShoot->Stop();
			oneBallShoot->Reset();
			twoBallShoot->Stop();
			twoBallShoot->Reset();
			
			prevTrigState = false;
			prevBumpState = false;
			prevOtherXState = false;
			intakeOn = false;
			willShoot = false;
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO){
			switch(taskState) {
			case WAIT_FOR_AUTO_INPUT:
				suction->Set(suction->kOff);
				break;
			case AUTONOMOUS_SHOOT:			
				suction->Set(suction->kForward);
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
				break;
			default:
				break;
			}
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////		
		if (taskStatus == STATUS_TELEOP) {	
			// Rollers // 
			if(intakeOn) {
				suction->Set(suction->kForward);
			}
			
			if(global->SecondaryGetButtonY() && intakeOn == true) {
				suction->Set(suction->kReverse); 
			} 
			
			//Hold down
			if(global->SecondaryGetRightBumper()){
				willShoot = true;
			} else if(twoBallShoot->Get() == 0 && oneBallShoot->Get() == 0 ){
				willShoot = false;
			}
			
			//Two Ball
			if(global->SecondaryGetLeftTrigger() && !prevTrigState) {
				willShoot = true;
				twoBallShoot->Start();
			} 
			
			if(twoBallShoot->Get() >= global->ReadCSV("TWO_BALL_SHOOT")) {
				willShoot = false;
				twoBallShoot->Stop();
				twoBallShoot->Reset();
			}
			
			//One ball
			if(global->SecondaryGetLeftBumper() && !prevBumpState) { 
				willShoot = true;
				oneBallShoot->Start();
			} 
			
			if(oneBallShoot->Get() >= global->ReadCSV("ONE_BALL_SHOOT")) {
				willShoot = false;
				oneBallShoot->Stop();
				oneBallShoot->Reset();
			}

			if(global->SecondaryGetButtonX() && !prevOtherXState) { //Toggle structure
				if(intakeOn) {
					intakeOn = false;
					suction->Set(suction->kOff);
				} else {
					intakeOn = true;
					suction->Set(suction->kForward);
				}
			}	
			
			if(willShoot){
				backplateEngage->Set(true);
				backplateDisengage->Set(false);
			}else {
				backplateEngage->Set(false);
				backplateDisengage->Set(true);				
			}
				
			// Arm control //
			if(global->PrimaryGetRightTrigger()) {
				armUp->Set(false);
				armDown->Set(true);
			} else {
				armUp->Set(true);
				armDown->Set(false);
			}	
			
			prevOtherXState = global->SecondaryGetButtonX();
			prevTrigState = global->SecondaryGetLeftTrigger();
			prevBumpState = global->SecondaryGetLeftBumper();
		}
		SwapAndWait();
	}

	return 0;
}
