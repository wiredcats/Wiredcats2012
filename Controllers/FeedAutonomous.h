#ifndef FEEDAUTONOMOUS_H_
#define FEEDAUTONOMOUS_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"

#include "../Tasks/Intake2415.h"
#include "../Tasks/Turret2415.h"

#define START (10)
#define WAIT (20)
#define SHOOT (30)
#define NEW_TURRET (40)
#define LAST_TURRET (50)
#define END (9000)

class FeedAutonomous2415 : public Task2415 {
private:
	Global *global;
	
	Task2415 *intake, *turret;
	
	Timer *waitTimer, *totalTimer;

public:
	FeedAutonomous2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

