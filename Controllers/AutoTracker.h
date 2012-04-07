#ifndef AUTOTRACKER_H_
#define AUTOTRACKER_H_

#include "WPILib.h"

#include "../Tasks/Turret2415.h"
#include "../Tasks/Intake2415.h"

#include "../Task2415.h"
#include "../Global.h"

#define SEARCH_FOR_BEST (10)
#define LOCK_AND_FOLLOW (20)

#define MANUAL_CONTROL (5)

class AutoTracker2415 : public Task2415 {
private:
	Global *global;	

	Task2415 *turret;
	
	Encoder* turretEncoder;
	
public:
	AutoTracker2415();
	
	virtual int Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
	void FindRatioGood(vector<ParticleAnalysisReport>* vec);
	ParticleAnalysisReport FindRatioBest(vector<ParticleAnalysisReport>* vec);
	double SideRatio(ParticleAnalysisReport vec);
	double AreaRatio(ParticleAnalysisReport vec);
	int BoundingBoxCenterX(ParticleAnalysisReport vec);
	ParticleAnalysisReport SearchForCloseFit(vector<ParticleAnalysisReport>* vec, int prevCentX, int prevCentY);
	double Distance(int Ax, int Ay, int Bx, int By);
};

#endif

