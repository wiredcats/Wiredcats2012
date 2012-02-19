#ifndef INTAKE2415_H_
#define INTAKE2415_H_

#include "WPILib.h"
#include "../Task2415.h"
#include "../Global.h"

class Intake2415 : public Task2415 {
private:
	Global *global;
	Solenoid *armUp, *armDown, *backplateEngage, *backplateDisengage;
	Relay *suction;

public:
	Intake2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif
