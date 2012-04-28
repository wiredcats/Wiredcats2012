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
	
	fortyFive = new Solenoid(1,1);
	sixty = new Solenoid(1,2);
	
	//For practice bot, (1,2...)
	//For real bot, (2,1...)
	wheelEncoder = new Encoder(2,1,false,(CounterBase::EncodingType)0); //0 maps to k1X. Shouldn't have to do this either...

	Start("turret2415");
}

int Turret2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	
	bool prevTrigState;
	double power, integral, goal;
	
	PIDSpecific = 0.0;

	while (keepTaskAlive) {
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		
		if(taskStatus == STATUS_DISABLED) {
			global->ResetCSV();
			
			prevTrigState = true;
			
			goal = global->ReadCSV("FENDER_SHOOTER_ENCODER_SPEED");
			
			fortyFive->Set(true);
			sixty->Set(false);	
			
			power = 0.0;
			integral = 0.0;
			PIDSpecific = 0.0;
			
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
			double goal;
			
			switch(taskState){
			case AUTO_FIRST_BALLS:
				goal = global->ReadCSV("KEY_SHOOTER_ENCODER_SPEED");
				break;
			case AUTO_SECOND_BALLS:
				goal = global->ReadCSV("SECONDARY_AUTO_SPEED");
				break;
			case AUTO_FINAL_BALLS:
				goal = global->ReadCSV("TERTIARY_AUTO_SPEED");
				break;
			default:
				break;
			}
			
			double error = goal - current;		
			integral+=error;

			printf("Current: %g, Goal: %g\n",current, goal);			
				
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
			if(global->SecondaryGetRightTrigger()){
				fortyFive->Set(true);
				sixty->Set(false);	
				goal = global->ReadCSV("KEY_SHOOTER_ENCODER_SPEED");
			} else {
				fortyFive->Set(false);
				sixty->Set(true);	
				goal = global->ReadCSV("FENDER_SHOOTER_ENCODER_SPEED");
			}
			
			// PID LOOP OF AWESOMENESS //			
			double current = wheelEncoder->GetRate();
//			printf("Current: %g, Goal: %g\n",current, goal);
			double error = goal - current;
			value = error;
			
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
			vicRotate->Set(PIDSpecific);
			
			prevTrigState = global->SecondaryGetRightTrigger();
		}
		SwapAndWait();
	}

	return 0;
}

