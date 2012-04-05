#ifndef SHOOTCONTROLLER_H_
#define SHOOTCONTROLLER_H_

#include "WPILib.h"

#include "../Tasks/Turret2415.h"
#include "../Tasks/Intake2415.h"

#include "../Task2415.h"
#include "../Global.h"

#define WAIT_FOR_INPUT (10)

const float ERROR_MARGIN = 250;

class ShootController2415 : public Task2415 {
private:
	Global *global;

	Task2415 *turret, *intake;

public:
	ShootController2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

