#ifndef WIREDCATS2415_H_
#define WIREDCATS2415_H_

#include "WPILib.h"
#include "Task2415.h"
#include "Global.h"
#include <math.h>
#include "Tasks.h"

class Wiredcats2415 : public SimpleRobot {
private:
	Compressor *compressor;

public:
	Wiredcats2415(void);
	
	void Disabled();
	void Autonomous();
	void OperatorControl();
};

#endif

