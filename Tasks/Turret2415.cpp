#include "Turret2415.h"

Turret2415::Turret2415() {
	global = new Global();
	
	vicWheel = new Victor(3);
	vicRotate = new Victor(4);
	
	fortyFive = new Solenoid(1);
	sixty = new Solenoid(2);
	
	wheelEncoder = new Encoder(1,2,false,(CounterBase::EncodingType)0); //0 maps to k1X. Shouldn't have to do this either...

	taskState = WAIT_FOR_INPUT;
	
	limitLeft = new DigitalInput(6);
	limitRight = new DigitalInput(5);

	Start("turret2415");
}

int Turret2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	wheelEncoder->Start();
	while (keepTaskAlive) {
		if (taskStatus == STATUS_AUTO || taskStatus == STATUS_TELEOP) {
			switch (taskState) {
				case WAIT_FOR_INPUT: 
					vicRotate->Set(0.0); 
					vicWheel->Set(global->ReadCSV("SHOOTER_WHEEL_SPEED"));
					fortyFive->Set(true);
					sixty->Set(false);					
					break;
				case MOVE_LEFT:
					vicRotate->Set(-(global->ReadCSV("TURRET_SPEED")));
//					if(limitLeft->Get()){
//						printf("Left limit hit\n");
//						vicRotate->Set(0.0);
//						taskState = WAIT_FOR_INPUT;
//					}			
					break;
				case MOVE_RIGHT:
					vicRotate->Set(global->ReadCSV("TURRET_SPEED"));
//					if(limitRight->Get()){
//						printf("Right limit hit\n");
//						vicRotate->Set(0.0);
//						taskState = WAIT_FOR_INPUT;
//					}
					break;
				case SHOOT:
					fortyFive->Set(false);
					sixty->Set(true);
				case PID_SPECIFIC:
					vicRotate->Set(PIDSpecific);
//					if(limitRight->Get() && global->GetTurretSpecific() > 0){
//						printf("Right limit hit\n");
//						vicRotate->Set(0.0);
//						taskState = WAIT_FOR_INPUT;
//					}
//					if(limitLeft->Get() && global->GetTurretSpecific() < 0){
//						printf("Left limit hit\n");
//						vicRotate->Set(0.0);
//						taskState = WAIT_FOR_INPUT;
//					}			
					break;
				default:
					vicRotate->Set(0.0);
					vicWheel->Set(0.0);
					break;
			}
		}
		SwapAndWait();
	}

	return 0;
}

