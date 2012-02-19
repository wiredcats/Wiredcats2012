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
            
    while (keepTaskAlive) {    	
        Threshold threshold(0,255,0,255,(int)global->ReadCSV("LUMINANCE_LOW"),255);
        ParticleFilterCriteria2 filter[] = {
    			{IMAQ_MT_AREA, 0, global->ReadCSV("PARTICLE_AREA_UPPER_BOUND"), false, false}
        };
        ParticleFilterOptions2 options[] = {true,false,false,true};
		
        if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			switch (taskState) {
				case WAIT_FOR_INPUT:
					if(global->GetButtonX()){
						taskState = MANUAL_CONTROL;
					}
					
					turret->SetState(WAIT_FOR_INPUT);
					if(camera.IsFreshImage()) {
						double degreesOff = 0.0;
						int targetHeight = 0;
						HSLimage = camera.GetImage();
						BWimage = HSLimage->ThresholdHSL(threshold);
						imaqImage = BWimage->GetImaqImage();
						imaqParticleFilter4(imaqImage,imaqImage,filter,1,options,NULL,NULL);
						imaqConvexHull(imaqImage,imaqImage,true);
						vector<ParticleAnalysisReport>* s_particles(BWimage->GetOrderedParticleAnalysisReports());
						printf("Particles Found: %d\n", s_particles->size());
										
						if(s_particles->size() > 0) {
							ParticleAnalysisReport temp = FindBest(s_particles);
							if(fabs(Ratio(temp)- 4.0/3.0) <= global->ReadCSV("TARGET_MARGIN_OF_ERROR")) { //Ratio is not the only measurement, when tilted, can be as far off as 1
								double degreesOff = -(2.0 / 47.0) * ((temp.imageWidth / 2.0) - temp.center_mass_x); //FOV of the M1011 is 47 degrees. Run PID loop on this to 0
								targetHeight = temp.imageHeight - temp.center_mass_y;
								printf("Best:    X:%d      Y:%d      Ratio:%g      Deg:%g   Height:%d\n", temp.center_mass_x, temp.center_mass_y, Ratio(temp), degreesOff, targetHeight);
							} else {
								degreesOff = 0;
								printf("Closest:   X:%d      Y:%d      Ratio:%g\n", temp.center_mass_x, temp.center_mass_y, Ratio(temp));
							}
						}				
					}
					delete HSLimage;
					delete BWimage;	
					imaqDispose(imaqImage);
					break;
				case MANUAL_CONTROL:
					turret->SetState(WAIT_FOR_INPUT);
					if(global->GetDPadX() > 0) {
						turret->SetState(MOVE_LEFT);
					} else if(global->GetDPadX() < 0){
						turret->SetState(MOVE_RIGHT);
					}
					
					if(global->GetButtonX()){
						taskState = WAIT_FOR_INPUT;
					}
					
					break;
				default:
					taskState = WAIT_FOR_INPUT;
					turret->SetState(WAIT_FOR_INPUT);
					break;
			}
		}
		SwapAndWait();
	}
	return 0;
}

ParticleAnalysisReport AutoTracker2415::FindBest(vector<ParticleAnalysisReport>* vec){
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

