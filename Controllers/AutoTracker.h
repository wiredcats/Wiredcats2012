#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_

#include "WPILib.h"

#include "../Tasks/Turret2415.h"

#include "../Task2415.h"
#include "../Global.h"

#define WAIT_FOR_INPUT (10)

class AutoTracker2415 : public Task2415 {
private:
	Global *global;	

	Task2415 *turret;
	
public:
	AutoTracker2415(void);
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
	ParticleAnalysisReport FindBest(vector<ParticleAnalysisReport>* vec);
	double Ratio(vector<ParticleAnalysisReport>* vec, int a);
};

#endif

