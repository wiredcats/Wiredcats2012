#include "AutoTracker.h"
#include <nivision.h>
#include <math.h>

/*
 * Ideas for future tracking
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
 */

/*
struct ParticleComparer {
	int compare(ParticleAnalysisReport p1, ParticleAnalysisReport p2) {
		float p1Ratio = p1.boundingRect.width / p1.boundingRect.height;
	    float p2Ratio = p2.boundingRect.width / p2.boundingRect.height;
	    
	 	return(fabs(p1Ratio - 4.0/3.0) < fabs(p2Ratio - 4.0/3.0));
	}
}; 

return(fabs(p1Ratio - 4.0/3.0) <= fabs(p2Ratio - 4.0/3.0));

if(s_particles->size() > 0) {
	QuickSort(s_particles,0,s_particles->size());
	for (unsigned int i = 0; i < s_particles->size(); i++) {
		ParticleAnalysisReport temp = s_particles->at(i);
        double degreesOff = -(2.0 / 47.0) * ((temp.imageWidth / 2.0) - temp.center_mass_x); //FOV of the M1011 is 47 degrees. Run PID loop on this to 0
        printf("Particle %d ", i+1);
        printf("    X:%d      Y:%d      Degrees off Center:%g      Size: %g\n", temp.center_mass_x, temp.center_mass_y, degreesOff, temp.particleArea);
        }
	}	
	
degreesOff = -(2.0 / 47.0) * ((temp.imageWidth / 2.0) - temp.center_mass_x); //FOV of the M1011 is 47 degrees. Run PID loop on this to 0

*/

//sort(s_particles->begin(), s_particles->end(), pCompare); //???

AutoTracker2415::AutoTracker2415(void) {
	global = Global::GetInstance();

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
    Threshold threshold(0,255,0,255,100,255);
    
    while (keepTaskAlive) {
		if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {
			switch (taskState) {
				case WAIT_FOR_INPUT:
					turret->SetState(WAIT_FOR_INPUT);
					if(camera.IsFreshImage()) {
						double degreesOff = 0.0;
						HSLimage = camera.GetImage();
						BWimage = HSLimage->ThresholdHSL(threshold);
						imaqImage = BWimage->GetImaqImage();
						imaqConvexHull(imaqImage, imaqImage, TRUE);
						vector<ParticleAnalysisReport>* s_particles(BWimage->GetOrderedParticleAnalysisReports());
						printf("Particles Found: %d\n", s_particles->size());
										
						if(s_particles->size() > 0) {
							ParticleAnalysisReport temp = FindBest(s_particles);
							double tempW = temp.boundingRect.width;
							double tempH = temp.boundingRect.height;
							double tempRatio = tempW / tempH;
							if(fabs(tempRatio - 4.0/3.0) <= global->ReadCSV("TARGET_MARGIN_OF_ERROR")) {
								degreesOff = -(2.0 / 47.0) * ((temp.imageWidth / 2.0) - temp.center_mass_x); //FOV of the M1011 is 47 degrees. Run PID loop on this to 0
								printf("Best:    X:%d      Y:%d      Ratio:%g      Degrees Off: %g\n", temp.center_mass_x, temp.center_mass_y, tempRatio, degreesOff);
							} else {
								degreesOff = 0;
								printf("Closest:   X:%d      Y:%d      Ratio:%g\n", temp.center_mass_x, temp.center_mass_y, tempRatio);
							}
						}
						
						if(degreesOff >= global->ReadCSV("TURRET_MARGIN_OF_ERROR")) {
							turret->SetState(MOVE_LEFT);
						}
						
						if(degreesOff <= -(global->ReadCSV("TURRET_MARGIN_OF_ERROR"))) {
							turret->SetState(MOVE_RIGHT);
						}
						
						delete HSLimage;
						delete BWimage;
						imaqDispose(imaqImage);								
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
		if((fabs(Ratio(vec,best) - 4.0/3.0)) >= (fabs(Ratio(vec,i) - 4.0/3.0))) {
			best = i;
		}
	}
	return(vec->at(best));
}

double AutoTracker2415::Ratio(vector<ParticleAnalysisReport>* vec, int a) {
	double tempW = (vec->at(a)).boundingRect.width;
	double tempH = (vec->at(a)).boundingRect.height;
	return tempW / tempH;
}

