#ifndef DRIVE2415_H_
#define DRIVE2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"
#include <math.h>

#define NORMAL_JOYSTICK (10)
#define GO_STRAIGHT (20)
#define MOVE_BACK (30)
#define STOP_BOT (40)
#define GO_BACKDRIVE_STRAIGHT (50)
#define MOVE_BACKDRIVE_BACK (60)

const float PI = 3.1415926;

class Drive2415 : public Task2415 {
private:
	Global *global;

	Victor *vicLeft, *vicRight;

public:
	Drive2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

