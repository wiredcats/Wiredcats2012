#include "AutoTracker.h"
#include <nivision.h>
#include <math.h>

AutoTracker2415::AutoTracker2415() {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);
	turret = Task2415::SearchForTask("turret2415");
	intake = Task2415::SearchForTask("intake2415");
	
	taskState = MANUAL_CONTROL;

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
    double integral = 0.0;
            
    while (keepTaskAlive) {    	        
        if(global->SecondaryGetButtonBack()) {
        	taskState = MANUAL_CONTROL;
        }
        
        if(global->SecondaryGetButtonStart()) {
        	taskState = SEARCH_FOR_BEST;
        }        	
        
        Threshold threshold(0,255,0,255,(int)global->ReadCSV("LUMINANCE_LOW"),255);
        ParticleFilterCriteria2 filter[] = {
    			{IMAQ_MT_AREA, 0, global->ReadCSV("PARTICLE_AREA_UPPER_BOUND"), false, false}
        };      
        
		//////////////////////////////////////
		// State: Disabled
		//////////////////////////////////////		        
        if(taskStatus == STATUS_DISABLED) {
        	global->ResetCSV();
        	integral = 0.0;
        }
        		
		//////////////////////////////////////
		// State: Auto / Teleop
		//////////////////////////////////////		
        if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
        	if(camera.IsFreshImage() && taskState != MANUAL_CONTROL) {
        		HSLimage = camera.GetImage();
				BWimage = HSLimage->ThresholdHSL(threshold);
				imaqImage = BWimage->GetImaqImage();
				imaqParticleFilter4(imaqImage,imaqImage,filter,1,options,NULL,NULL);
				imaqMorphology(imaqImage,imaqImage,IMAQ_DILATE,NULL);
				imaqMorphology(imaqImage,imaqImage,IMAQ_DILATE,NULL);
				imaqConvexHull(imaqImage,imaqImage,true);
				s_particles = BWimage->GetOrderedParticleAnalysisReports();
				
				delete HSLimage;
				delete BWimage;	
				imaqDispose(imaqImage);
        	}
        							
			switch (taskState) {
				case MANUAL_CONTROL:
					turret->SetPWMSpecific(-global->SecondaryGetLeftX() * 0.2);
					if(global->SecondaryGetDPadX() < 0) { //Move Left
						turret->SetPWMSpecific(global->ReadCSV("TURRET_SPEED"));
					}
					if(global->SecondaryGetDPadX() > 0){ //Move right
						turret->SetPWMSpecific(-global->ReadCSV("TURRET_SPEED"));
					}
					break;
				case SEARCH_FOR_BEST:	     					
				    turret->SetPWMSpecific(0.0);
					int targetHeight;
					targetHeight = 0;
					
					//TODO: Not sure this is right application of PID
				    integral = 0.0;
					
					printf("Particles Found!: %d\n", s_particles->size());										
					if(s_particles->size() > 0) {
						ratioBest = FindRatioBest(s_particles);
						if(fabs(SideRatio(ratioBest)- 4.0/3.0) <= global->ReadCSV("TARGET_MARGIN_OF_ERROR")) { //Ratio is not the only measurement, when tilted, can be as far off as 1
							targetHeight = ratioBest.imageHeight - ratioBest.center_mass_y;
							printf("Best:(%d,%d)    SideRatio:%g    Area:%g    AreaRatio:%g   Height:%d\n", ratioBest.center_mass_x, ratioBest.center_mass_y, SideRatio(ratioBest), ratioBest.particleArea, AreaRatio(ratioBest), targetHeight);
							taskState = LOCK_AND_FOLLOW;
							prevCentX = ratioBest.center_mass_x;
							prevCentY = ratioBest.center_mass_y;
						} else { //TODO: Find another method that isn't so concerned about ratio. Consistnently one target and stay lcoked on
							//TODO: Write PID Loop that centers the loop on closest one?
							//TODO: Or pick one where center of mass differ most from BB center of mass (most skew)? 
							//TODO: Closest in area???
							printf("Closest:(%d,%d)    SideRatio:%g    AreaRatio:%g\n", ratioBest.center_mass_x, ratioBest.center_mass_y, SideRatio(ratioBest),AreaRatio(ratioBest));
						}
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
								
								//PID Loop
								//Recall that the origin is the top left corner
								int currentCent = closest.center_mass_x;
								double error = closest.imageWidth / 2 - currentCent;
								double deriv = prevCentX - currentCent;
								
								//1771 suggests taking squareroot and multiplying by constant
//								error = pow(fabs(error),0.5) * 0.666;
								
								integral+=error;
									
								//For non squarerooted error (Distance of center of target from middle of image)
								// (0.0013, 0.000067, 0.0006)
								double kp = global->ReadCSV("KP_TURRET");
								double ki = global->ReadCSV("KI_TURRET");
								double kd = global->ReadCSV("KD_TURRET");
								
								printf("PID: (%g, %g, %g)\n",kp,ki,kd);
									
								// Compute the power to send to the arm.
								double power = kp * error + ki * integral + kd * deriv;
								
								printf("Error: %g, Power:%g\n",error,power);
								
								turret->SetPWMSpecific(power);			
																
								prevCentX = closest.center_mass_x;
								prevCentY = closest.center_mass_y;
								}
						} else {
							taskState = SEARCH_FOR_BEST;
						}					
					break;
				default:
					taskState = SEARCH_FOR_BEST;
					turret->SetPWMSpecific(0.0);
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
		if((fabs(SideRatio(vec->at(best)) - 4.0/3.0)) >= (fabs(SideRatio(vec->at(i)) - 4.0/3.0))) {
			if(AreaRatio(vec->at(i)) >= global->ReadCSV("AREA_RATIO_LOWER_BOUND")) {
				best = i;
			}
		}
	}
	return(vec->at(best));
}

double AutoTracker2415::SideRatio(ParticleAnalysisReport vec) {
	double tempW = vec.boundingRect.width;
	double tempH = vec.boundingRect.height;
	return tempW / tempH;
}

double AutoTracker2415::AreaRatio(ParticleAnalysisReport vec) {	
	double bbArea = vec.boundingRect.height * vec.boundingRect.width;
	return (vec.particleArea / bbArea);
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
