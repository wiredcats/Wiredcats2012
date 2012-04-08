#ifndef TURRET2415_H_
#define TURRET2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"
#include <math.h>

#define AUTO_FIRST_BALLS (7)
#define AUTO_REST_BALLS (17)
#define AUTO_FINAL_BALLS (27)

class Turret2415 : public Task2415 {
private:
	Global *global;
	
	Victor *vicWheel, *vicRotate;
	
	Solenoid *fortyFive, *sixty;
	
	Encoder *wheelEncoder;	
public:
	Turret2415();
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

