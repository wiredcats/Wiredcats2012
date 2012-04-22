#ifndef BRIDGEAUTONOMOUS_H_
#define BRIDGEAUTONOMOUS_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"

#include <math.h>

#include "../Tasks/Turret2415.h"
#include "../Tasks/Intake2415.h"
#include "../Tasks/Drive2415.h"

#define START (10)
#define WAIT (20)
#define SHOOT (30)
#define DRIVE_BACK (40)
#define DRIVE_FORWARD (50)
#define INTAKE_BALLS (60)
#define END (9000)

class BridgeAutonomous2415 : public Task2415 {
private:
	Global *global;
	Task2415 *turret, *intake, *drive;
	Timer *waitTimer;
	Encoder *driveEncoder;
	Gyro *gyro;
public:
	BridgeAutonomous2415(void);

	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif
