#include "AutoTracker.h"
#include <nivision.h>
#include <math.h>

/*
 * TODO: Ideas for future tracking
 * ----------------------------
 * To lock on to the target, once already found it, 
 * pick the target which had center of mass closest to previous locked on target.
 * 
 * If lose track of image, make turret move to look for target and repeat original ideal vision track.
 * 
 * When want to center turret on target
 * Do PID loop for the imgwidth / 2 - C_x
 * 
 * When do distance calculations
 * Check out the bounding rectangle length / width compared to standard.
 * 
 * Do distance calculations for shooter. Only do calculations when toggle button
 * This calculation will set the flywheel speed at a certain speed to the right speed for distance
 *
 * Toggle button so drivers can pick when they want to autotarget
 */

AutoTracker2415::AutoTracker2415(void) {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);
	turret = Task2415::SearchForTask("turret2415");
	
	taskState = SEARCH_FOR_BEST;

	Start("autotracker2415");
}

int AutoTracker2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	AxisCamera &camera = AxisCamera::GetInstance("10.24.15.11");
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteColorLevel(0);
	camera.WriteCompression(100);
	camera.WriteBrightness(0);
	
    HSLImage* HSLimage;
    BinaryImage* BWimage;
    Image* imaqImage;
    
    ParticleFilterOptions2 options[] = {true,false,false,true};
    ParticleAnalysisReport ratioBest;
    vector<ParticleAnalysisReport>* s_particles;
    
    int prevCentX, prevCentY;
            
    while (keepTaskAlive) {    	
        Threshold threshold(0,255,0,255,(int)global->ReadCSV("LUMINANCE_LOW"),255);
        ParticleFilterCriteria2 filter[] = {
    			{IMAQ_MT_AREA, 0, global->ReadCSV("PARTICLE_AREA_UPPER_BOUND"), false, false}
        };      
        
        if(global->GetButtonBack()) {
        	taskState = MANUAL_CONTROL;
        }
        
        if(global->GetButtonStart()) {
        	taskState = SEARCH_FOR_BEST;
        }
		
        if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
        	if(camera.IsFreshImage()) {
        		HSLimage = camera.GetImage();
				BWimage = HSLimage->ThresholdHSL(threshold);
				imaqImage = BWimage->GetImaqImage();
				imaqParticleFilter4(imaqImage,imaqImage,filter,1,options,NULL,NULL);
				imaqConvexHull(imaqImage,imaqImage,true);
				s_particles = BWimage->GetOrderedParticleAnalysisReports();
				
				delete HSLimage;
				delete BWimage;	
				imaqDispose(imaqImage);
        	}
        							
			switch (taskState) {
				case SEARCH_FOR_BEST:
				    bool bestFound;
				    bestFound = false;
				    
					if(global->GetButtonX()){
						taskState = MANUAL_CONTROL;
					}
					turret->SetState(WAIT_FOR_INPUT);
					double degreesOff;
					int targetHeight;
					degreesOff = 0.0;
					targetHeight = 0;
					
					printf("Particles Found!: %d\n", s_particles->size());										
					if(s_particles->size() > 0) {
						ratioBest = FindRatioBest(s_particles);
						if(fabs(Ratio(ratioBest)- 4.0/3.0) <= global->ReadCSV("TARGET_MARGIN_OF_ERROR")) { //Ratio is not the only measurement, when tilted, can be as far off as 1
							double degreesOff = -(2.0 / 47.0) * ((ratioBest.imageWidth / 2.0) - ratioBest.center_mass_x); //FOV of the M1011 is 47 degrees. Run PID loop on this to 0
							targetHeight = ratioBest.imageHeight - ratioBest.center_mass_y;
							printf("Best:    X:%d      Y:%d      Ratio:%g      Deg:%g   Height:%d\n", ratioBest.center_mass_x, ratioBest.center_mass_y, Ratio(ratioBest), degreesOff, targetHeight);
							taskState = LOCK_AND_FOLLOW;
							prevCentX = ratioBest.center_mass_x;
							prevCentY = ratioBest.center_mass_y;
						} else { //TODO: Find another method that isn't so concerned about ratio. Consistnently one target and stay lcoked on
							//TODO: Write PID Loop that centers the loop on closest one?
							//TODO: Or pick one where center of mass differ most from BB center of mass (most skew)? 
							printf("Closest:   X:%d      Y:%d      Ratio:%g\n", ratioBest.center_mass_x, ratioBest.center_mass_y, Ratio(ratioBest));
						}
					}
					break;
				case MANUAL_CONTROL:
					turret->SetState(WAIT_FOR_INPUT);
					if(global->GetDPadX() > 0) {
						turret->SetState(MOVE_LEFT);
					} else if(global->GetDPadX() < 0){
						turret->SetState(MOVE_RIGHT);
					}
					
					if(global->GetButtonY()){
						taskState = SEARCH_FOR_BEST;
					}
					break;
				case LOCK_AND_FOLLOW:
					///From previous image
					//Find current particle that is closest to center of mass
					//to that particle
				printf("\nLocked on - Particles found: %d\n", s_particles->size());
					if(s_particles->size() > 0) {
						ParticleAnalysisReport closest = SearchForCloseFit(s_particles, prevCentX, prevCentY);
						printf("Previous: (%d,%d)  Current: (%d,%d)   Dist: %g\n",prevCentX, prevCentY, closest.center_mass_x, closest.center_mass_y, Distance(prevCentX, prevCentY, closest.center_mass_x, closest.center_mass_y));
						if(Distance(prevCentX, prevCentY, closest.center_mass_x, closest.center_mass_y) > global->ReadCSV("TARGET_DIST_MARGIN")){
							taskState = SEARCH_FOR_BEST;
							printf("Stop locked\n");
						} else {
							printf("Continue locked\n");
							taskState = LOCK_AND_FOLLOW;
							prevCentX = closest.center_mass_x;
							prevCentY = closest.center_mass_y;
							if(-(2.0 / 47.0) * ((closest.imageWidth / 2.0) - closest.center_mass_x) > 0) {
								turret->SetState(MOVE_LEFT);
							} else {
								turret->SetState(MOVE_RIGHT);
							}
						}
					} else {
						taskState = SEARCH_FOR_BEST;
					}					
					break;
				default:
					taskState = SEARCH_FOR_BEST;
					turret->SetState(WAIT_FOR_INPUT);
					break;
			}
		}
		SwapAndWait();
	}
	return 0;
}

ParticleAnalysisReport AutoTracker2415::FindRatioBest(vector<ParticleAnalysisReport>* vec){
	int size = vec->size();
	int best = 0;
	
	for(int i = 1; i < size; i++) {
		if((fabs(Ratio(vec->at(best)) - 4.0/3.0)) >= (fabs(Ratio(vec->at(i)) - 4.0/3.0))) {
			best = i;
		}
	}
	return(vec->at(best));
}

double AutoTracker2415::Ratio(ParticleAnalysisReport vec) {
	double tempW = vec.boundingRect.width;
	double tempH = vec.boundingRect.height;
	return tempW / tempH;
}

int AutoTracker2415::BoundingBoxCenterX(ParticleAnalysisReport vec) {
	return(vec.boundingRect.left + vec.boundingRect.width / 2);
}

ParticleAnalysisReport AutoTracker2415::SearchForCloseFit(vector<ParticleAnalysisReport>* vec, int prevCentX, int prevCentY) {
	int size = vec->size();
	double bestDist = 999;
	double dist;
	int best = 0;
	
	for(int i = 0; i < size; i++) {
		dist = Distance(prevCentX, prevCentY, vec->at(i).center_mass_x, vec->at(i).center_mass_y);
		if((dist) < (bestDist)) {
			best = i;
			bestDist = dist;
		}
	}
	return(vec->at(best));
}

double AutoTracker2415::Distance(int Ax, int Ay, int Bx, int By) {
	return(sqrt(pow(Ax - Bx,2) + pow(Ay - By,2)));
}
