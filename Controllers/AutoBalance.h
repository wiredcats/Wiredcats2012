#ifndef AUTOBALANCE_H_
#define AUTOBALANCE_H_

#include "WPILib.h"

#include "../Tasks/Drive2415.h"

#include "../Task2415.h"
#include "../Global.h"

#define WAIT_FOR_INPUT (30)

class AutoBalance2415 : public Task2415 {
private:
	Global *global;

	Task2415 *drive;

public:
	AutoBalance2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
};

#endif

