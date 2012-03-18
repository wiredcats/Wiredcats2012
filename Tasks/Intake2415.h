#ifndef INTAKE2415_H_
#define INTAKE2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"

#define AUTONOMOUS_SHOOT (7)
#define WAIT_FOR_AUTO_INPUT (17)

#define DISENGAGE (10)
#define ENGAGE (20)
#define WAIT_FOR_INPUT (30)

class Intake2415 : public Task2415 {
private:
	Global *global;
	Solenoid *armUp, *armDown, *backplateEngage, *backplateDisengage;
	Relay *suction;	
	Timer *oneBallShoot, *twoBallShoot;

public:
	Intake2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

