#ifndef DRIVE2415_H_
#define DRIVE2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"
#include <math.h>

#define NORMAL_JOYSTICK (10)
#define AUTOBALANCE (20)

#define FORWARD (7)
#define BACK (17)
#define TURN_FORWARD_RIGHT (27)
#define TURN_FORWARD_LEFT (37)
#define TURN_BACKWARD_RIGHT (47)
#define TURN_BACKWARD_LEFT (57)

const float STRAIGHT_SPEED = 0.3;
const float TURN_SPEED = 0.2;

const float PI = 3.1415926;

class Drive2415 : public Task2415 {
private:
	Global *global;

	Victor *vicLeft, *vicRight;
	
	Solenoid *brakeOff, *brakeNow;
	
	Timer *stupidTimer;

public:
	Drive2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif
