#ifndef TURRET2415_H_
#define TURRET2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"
#include <math.h>

#define AUTO_FIRE (7)

#define FENDER_SHOOT (10)
#define KEY_SHOOT (15)

//TODO: Put this into the text document
//We will end up having 2 CSV for each robot
const double LEFT_LIMIT = 1.88;
const double RIGHT_LIMIT = 4.79;

class Turret2415 : public Task2415 {
private:
	Global *global;
	
	Victor *vicWheel, *vicRotate;
	
	Solenoid *fortyFive, *sixty;
	
	Encoder *wheelEncoder;
	
	AnalogChannel *pot;
	
public:
	Turret2415();
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

