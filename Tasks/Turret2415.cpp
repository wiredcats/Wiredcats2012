#include "Turret2415.h"

Turret2415::Turret2415() {
	global = new Global();
	
	vicWheel = new Victor(3);
	vicRotate = new Victor(4);
	
	fortyFive = new Solenoid(2);
	sixty = new Solenoid(1);
	
	wheelEncoder = new Encoder(1,2,false,(CounterBase::EncodingType)0); //0 maps to k1X. Shouldn't have to do this either...

	taskState = WAIT_FOR_INPUT;
	
	pot = new AnalogChannel(1,3);

	Start("turret2415");
}

int Turret2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool prevTrigState, isSixty;
	double power, integral;

	while (keepTaskAlive) {				
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			prevTrigState = true;
			isSixty = false;
			
			fortyFive->Set(true);
			sixty->Set(false);	
			
			power = 0.0;
			integral = 0.0;
			
			wheelEncoder->Stop();
			wheelEncoder->Reset();
			wheelEncoder->Start();			
		}
		if (taskStatus == STATUS_AUTO || taskStatus == STATUS_TELEOP) {
			
//			printf("Pot: %g\n",pot->GetVoltage());			
			if(global->SecondaryGetRightTrigger() && !prevTrigState) {
				if(isSixty){
					isSixty = false;
					fortyFive->Set(true);
					sixty->Set(false);
				} else {
					isSixty = true;
					fortyFive->Set(false);
					sixty->Set(true);
				}
			}
			prevTrigState = global->SecondaryGetRightTrigger();
			
			//TODO: Do the weird wheel latency thing that Hailey wants (???)
			
			//IMPORTANT: 
			//For speed PID loop, you can't send negative voltage through the motor
			//This is a modified loop to take this into consideration
			//Also, note that the I constant acts like P
			//and the P constant acts like D
			double current = wheelEncoder->GetRate();
//			printf("Encoder: %g\n",current);
			double error = global->ReadCSV("FENDER_SHOOTER_ENCODER_SPEED") - current;
			
			integral+=error;
				
			double kp = global->ReadCSV("KP_FLYWHEEL");
			double ki = global->ReadCSV("KI_FLYWHEEL");
			
			double power = kp * error + ki * integral;
			
//			printf("Error: %g, Power:%g\n",error,power);					
			if(power > 0) {
				vicWheel->Set(power);		
			} else {
				vicWheel->Set(0.1);
			}
			
			switch (taskState) {
				case WAIT_FOR_INPUT: 
					vicRotate->Set(0.0); 
					break;
				case MOVE_LEFT:
					vicRotate->Set(global->ReadCSV("TURRET_SPEED"));
//					if(pot->GetVoltage() <= LEFT_LIMIT) {
//						vicRotate->Set(0.0);
//						printf("Left Limit hit\n");
//					}
					break;
				case MOVE_RIGHT:
					vicRotate->Set(-(global->ReadCSV("TURRET_SPEED")));
//					if(pot->GetVoltage() >= RIGHT_LIMIT) {
//						vicRotate->Set(0.0);
//						printf("Right Limit hit\n");
//					}
					break;
				case PID_SPECIFIC:
					vicRotate->Set(PIDSpecific);
//					if(pot->GetVoltage() >= RIGHT_LIMIT && PIDSpecific > 0) {
//						vicRotate->Set(0.0);
//						printf("Right Limit hit\n");
//					}
//					if(pot->GetVoltage() <= LEFT_LIMIT && PIDSpecific < 0) {
//						vicRotate->Set(0.0);
//						printf("Left Limit hit\n");
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

