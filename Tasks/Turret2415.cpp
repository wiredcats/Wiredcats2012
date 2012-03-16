#include "Turret2415.h"

//IMPORTANT: 
//For speed PID loop, you can't send negative voltage through the motor
//This is a modified loop to take this into consideration
//Also, note that the I constant acts like P
//and the P constant acts like D

Turret2415::Turret2415() {
	global = new Global();
	
	vicWheel = new Victor(3);
	vicRotate = new Victor(4);
	
	fortyFive = new Solenoid(1);
	sixty = new Solenoid(2);
	
	//For practice bot, (1,2...)
	//For real bot, (2,1...)
	wheelEncoder = new Encoder(2,1,false,(CounterBase::EncodingType)0); //0 maps to k1X. Shouldn't have to do this either...

	taskState = WAIT_FOR_INPUT;
	
	pot = new AnalogChannel(1,3);

	Start("turret2415");
}

int Turret2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool prevTrigState, prevAState, prevBState, prevYState;
	bool isSixty;
	double power, integral;

	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			prevTrigState = true;
			prevAState = true;
			prevBState = true;
			prevYState = true;
			isSixty = false;
			
			fortyFive->Set(true);
			sixty->Set(false);	
			
			power = 0.0;
			integral = 0.0;
			
			wheelEncoder->Stop();
			wheelEncoder->Reset();
			wheelEncoder->Start();			
		}
		
		//////////////////////////////////////
		// State: Autonomous
		//////////////////////////////////////		
		if(taskStatus == STATUS_AUTO){
			fortyFive->Set(true);
			sixty->Set(false);	
			double current = wheelEncoder->GetRate();
			printf("Encoder: %g\n",current);
			double error = global->ReadCSV("KEY_SHOOTER_ENCODER_SPEED") - current;
			
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
		}
		
		//////////////////////////////////////
		// State: Teleop
		//////////////////////////////////////				
		if (taskStatus == STATUS_TELEOP) {
//			printf("Pot: %g\n",pot->GetVoltage());			
			
			fortyFive->Set(false);
			sixty->Set(true);	
			
			// PID LOOP OF AWESOMENESS //
			
			double goal;
			goal = global->ReadCSV("FENDER_SHOOTER_ENCODER_SPEED");
			
			double current = wheelEncoder->GetRate();
//			printf("Encoder: %g\n",current);
			double error = goal - current;
			
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
			
			// Turret movement //
			
			switch (taskState) {
				case WAIT_FOR_INPUT: 
					vicRotate->Set(0.0); 
					break;
				case MOVE_LEFT:
					vicRotate->Set(global->ReadCSV("TURRET_SPEED"));
					break;
				case MOVE_RIGHT:
					vicRotate->Set(-(global->ReadCSV("TURRET_SPEED")));
					break;
				case PID_SPECIFIC:
					vicRotate->Set(PIDSpecific);
					break;
				default:
					vicRotate->Set(0.0);
					vicWheel->Set(0.0);
					break;
			}
			
			prevAState = global->SecondaryGetButtonA();
			prevBState = global->SecondaryGetButtonB();
			prevYState = global->SecondaryGetButtonY();
		}
		SwapAndWait();
	}

	return 0;
}

