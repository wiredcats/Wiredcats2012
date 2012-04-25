#include "BridgeAutonomous.h"

BridgeAutonomous2415::BridgeAutonomous2415(void) {
	global = new Global();

	turret = Task2415::SearchForTask("turret2415");
	intake = Task2415::SearchForTask("intake2415");
	drive = Task2415::SearchForTask("drive2415");

	waitTimer = new Timer();

	printf("stalling to allow tasks to be initialized\n");
	Wait(1.0);

	driveEncoder = new Encoder(12,13,false,(CounterBase::EncodingType)2);
	gyro = new Gyro(1);

	taskState = START;

	Start("bridgeautonomous2415");
}

int BridgeAutonomous2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	while (keepTaskAlive) {
		if(taskStatus == STATUS_DISABLED){
			global->ResetCSV();
			intake->SetState(WAIT_FOR_AUTO_INPUT);
			waitTimer->Stop();
			waitTimer->Reset();
			gyro->Reset();
			driveEncoder->Stop();
			driveEncoder->Reset();
			taskState = START;
			}
		
		//TODO: Have 2 distinct phases
		//When far from the bridge, TURN_SPEED = 0.2, TURN_GAIN = 0.15
		//When close to the bridge (know by encoder value), TURN_SPEED = 0.0. TURN_GAIN = 0.5
		
		if (taskStatus == STATUS_AUTO) {
			//Wait for a bit and then shoot with key shot
			switch(taskState) {
			case START:
				waitTimer->Start();
				turret->SetState(AUTO_FIRST_BALLS);
				taskState = WAIT;
				break;
			case WAIT:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME") ) {
					taskState = DRIVE_FORWARD;
					driveEncoder->Start();
					gyro->Reset();
					waitTimer->Stop();
					waitTimer->Reset();
				}
				break;
			case DRIVE_FORWARD:
//				printf("Encoder: %d, Gyro:%g \n",driveEncoder->Get(),gyro->GetAngle());
				drive->SetState(FORWARD);
				if(gyro->GetAngle() >= global->ReadCSV("MARGIN_OF_ANGLE")){
					driveEncoder->Stop();
					drive->SetState(TURN_FORWARD_RIGHT);
				} else if(gyro->GetAngle() <= -global->ReadCSV("MARGIN_OF_ANGLE")){
					driveEncoder->Stop();
					drive->SetState(TURN_FORWARD_LEFT);
				} else {
					driveEncoder->Start();
				}
//				if(driveEncoder->Get() <= -global->ReadCSV("COUNTS_DRIVE")) {
//					taskState = INTAKE_BALLS;
//					waitTimer->Start();
//					driveEncoder->Stop();
//					driveEncoder->Reset();
//					drive->SetState(NORMAL_JOYSTICK);
//				}
				break;
			case INTAKE_BALLS:
				if(waitTimer->Get() >= global->ReadCSV("AUTONOMOUS_WAIT_TIME") ) {
					taskState = DRIVE_BACK;
					waitTimer->Stop();
					waitTimer->Reset();
					driveEncoder->Start();
				}
				break;
			case DRIVE_BACK:
//				printf("Encoder: %d, Gyro:%g \n",driveEncoder->Get(),gyro->GetAngle());
				drive->SetState(BACK);
				if(gyro->GetAngle() >= global->ReadCSV("MARGIN_OF_ANGLE")){
					driveEncoder->Stop();
					drive->SetState(TURN_BACKWARD_LEFT);
				} else if(gyro->GetAngle() <= -global->ReadCSV("MARGIN_OF_ANGLE")){
					driveEncoder->Stop();
					drive->SetState(TURN_BACKWARD_RIGHT);
				} else {
					driveEncoder->Start();
				}
				if(driveEncoder->Get() >= global->ReadCSV("COUNTS_DRIVE")) {
					taskState = SHOOT;
					drive->SetState(NORMAL_JOYSTICK);
				}
				break;
			case SHOOT:
				intake->SetState(SHOOT);
				taskState = END;
				break;
			case END:
				break;
			default:
				break;
			}
		}
		SwapAndWait();
	}
	
	return 0;
	}
