#ifndef Autonomous_H_
#define Autonomous_H_

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

//TODO: Put all of these pesky constants into the text file soon!
const float MARGIN_OF_ANGLE = 0.75;
const int COUNTS_DRIVE = 350;

class Autonomous2415 : public Task2415 {
private:
	Global *global;
	
	Task2415 *turret, *intake, *drive;
	
	Timer *waitTimer;
	Encoder *driveEncoder;
	Gyro *gyro;

public:
	Autonomous2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

