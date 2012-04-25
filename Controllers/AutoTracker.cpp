#include "AutoTracker.h"
#include <nivision.h>
#include <math.h>

AutoTracker2415::AutoTracker2415() {
	global = new Global();

	printf("stalling to allow tasks to be initialized\n");
	Wait(2.0);
	turret = Task2415::SearchForTask("turret2415");
	
	turretEncoder = new Encoder(8,9,false,(CounterBase::EncodingType)0); 

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
    vector<ParticleAnalysisReport>* s_particles;
    
    double integral = 0.0;
    int current, goal, error, prev, deriv;
            
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
        	turretEncoder->Stop();
        	turretEncoder->Reset();
        	turretEncoder->Start();
        }
        		
		//////////////////////////////////////
		// State: Auto / Teleop
		//////////////////////////////////////		
        if (taskStatus == STATUS_TELEOP || taskStatus == STATUS_AUTO) {  
            if(global->SecondaryGetButtonBack()) {
            	taskState = MANUAL_CONTROL;
            }
            
            if(global->SecondaryGetButtonStart()) {
            	taskState = SEARCH_FOR_BEST;
            }        	
        	
        	if(camera.IsFreshImage() && taskState == SEARCH_FOR_BEST) {
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
					current = turretEncoder->Get();
//					printf("Current Manual: %d\n ", current);
					turret->SetPWMSpecific(-global->SecondaryGetLeftX() * 0.175);
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
					
				    integral = 0.0;
					
					printf("Particles Found!: %d\n", s_particles->size());										
					if(s_particles->size() > 0) {
						FindRatioGood(s_particles);
					}
					if(s_particles->size() == 3) {
						targetHeight = MidHeight(s_particles);
						
						//6.783 pixels / degree 
						//2.553 clicks / degree 
						// TODO: Tune these constants more accurately
						// 2.553 / 6.783 = 0.3714
						goal = (int) (current + global->ReadCSV("TURRET_BIAS") *(160 - AverageX(s_particles) - global->ReadCSV("TURRET_GAIN")));
						printf("Best: Xcor = %d   TargetHeight = %d\n",AverageX(s_particles), targetHeight);
						current = turretEncoder->Get();						
						prev = current;
						printf("Current Found: %d         Goal: %d\n ", current, goal);
						taskState = LOCK_AND_FOLLOW;
					}
					break;
				case LOCK_AND_FOLLOW:
					//Based on encoder now					
					//PID Loop
					//Recall that the origin is the top left corner
					current = turretEncoder->Get();
//					printf("Current Locked: %d\n ", current);
					error = goal - current;
					deriv = prev - current;
										
					integral+=error;
					
					double kp = global->ReadCSV("KP_TURRET");
					double ki = global->ReadCSV("KI_TURRET");
					double kd = global->ReadCSV("KD_TURRET");
					
					printf("PID: (%g, %g, %g)\n",kp,ki,kd);
					
					if(error != 0) {				
						// Compute the power to send to the arm.
						double power = kp * error + ki * integral + kd * deriv;
						printf("Error: %d, Power:%g\n",error,power);						
						turret->SetPWMSpecific(power);																	
						prev = current;
						taskState = LOCK_AND_FOLLOW;
					} else {
						printf("Current: %d, Goal: %d",current,goal);
						taskState = MANUAL_CONTROL;
					}
					
					break;
				default:
					taskState = MANUAL_CONTROL;
					turret->SetPWMSpecific(0.0);
					break;
			}
		}
		SwapAndWait();
	}
	return 0;
}

void AutoTracker2415::FindRatioGood(vector<ParticleAnalysisReport>* vec){
	int size = vec->size();
	
//	|| AreaRatio(vec->at(i)) <= global->ReadCSV("AREA_RATIO_LOWER_BOUND")
	
	for(int i = 0; i < size; i++) {
//		printf("Target %d/%d: (%d,%d)   SideRatio: %g     Area: %g    AreaRatio:%g\n",i, size, vec->at(i).center_mass_x, vec->at(i).center_mass_y, SideRatio(vec->at(i)), vec->at(i).particleArea, AreaRatio(vec->at(i)));
		if((fabs(SideRatio(vec->at(i)) - 4.0/3.0)) >= global->ReadCSV("TARGET_MARGIN_OF_ERROR")) {
			if(size != 1) {
				vec->erase(vec->begin() + i);
				i--;
			} else {
				break;
			}
		}
		size = vec->size();
	}
//	printf("\n\n");
}

int AutoTracker2415::MidHeight(vector<ParticleAnalysisReport>* vec){
	int size = vec->size();	
	int highest = 0, total = 0;
	
	for(int i = 0; i < size; i++){
		total += vec->at(i).center_mass_y;
		if(vec->at(i).center_mass_y < vec->at(highest).center_mass_y) {
			highest = i;
		}
	}
	total -= vec->at(highest).center_mass_y;
	return(abs(vec->at(highest).center_mass_y - total / 2));
}

int AutoTracker2415::AverageX(vector<ParticleAnalysisReport>* vec){
	int size = vec->size();
	int total = 0;
	
	for(int i = 0; i < size; i++) {
		total += vec->at(i).center_mass_x;
	}
	return(total / size);
}

ParticleAnalysisReport AutoTracker2415::FindRatioBest(vector<ParticleAnalysisReport>* vec){
	int size = vec->size();
	int best = 0;
	
//	printf("Target 0: (%d,%d)   SideRatio: %g     Area: %g    AreaRatio:%g\n", vec->at(best).center_mass_x, vec->at(best).center_mass_y, SideRatio(vec->at(best)), vec->at(best).particleArea, AreaRatio(vec->at(best)));
	
	for(int i = 1; i < size; i++) {
//		printf("Target %d: (%d,%d)   SideRatio: %g     Area: %g    AreaRatio:%g\n",i, vec->at(i).center_mass_x, vec->at(i).center_mass_y, SideRatio(vec->at(i)), vec->at(i).particleArea, AreaRatio(vec->at(i)));
		if(vec->at(i).center_mass_y < vec->at(best).center_mass_y) {
			best = i;
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
