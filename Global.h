#ifndef GLOBAL_H
#define GLOBAL_H

#include "WPILib.h"
#include "CSV/CSVReader.h"

/*
 * Not the same glorified global as before
 * Now, it just has Joysticks, CSV readers
 * and other random stuff
 */

class Global{	
public:
	Global();
	
	//NOTE: DO NOT HIT MODE SWITCH. THIS WILL CHANGE LEFT Y INTO DPAD Y	
		
	double ReadCSV(const std::string& valueName);
	void ResetCSV();
	
	double LinearizeVictor(double goal_speed);
	
	//Primary Controller
	double PrimaryGetLeftX();
	double PrimaryGetLeftY();
	double PrimaryGetRightX();
	double PrimaryGetRightY();
	double PrimaryGetDPadX();
	
	bool PrimaryGetLeftBumper();
	bool PrimaryGetRightBumper();
	bool PrimaryGetLeftTrigger();
	bool PrimaryGetRightTrigger();
	
	bool PrimaryGetButtonA();
	bool PrimaryGetButtonB();
	bool PrimaryGetButtonX();
	bool PrimaryGetButtonY();
	
	bool PrimaryGetButtonBack();
	bool PrimaryGetButtonStart();
	
	bool PrimaryGetLeftPushDown();
	bool PrimaryGetRightPushDown();
	
	//Secondary Controller
	double SecondaryGetLeftX();
	double SecondaryGetLeftY();
	double SecondaryGetRightX();
	double SecondaryGetRightY();
	double SecondaryGetDPadX();
	
	bool SecondaryGetLeftBumper();
	bool SecondaryGetRightBumper();
	bool SecondaryGetLeftTrigger();
	bool SecondaryGetRightTrigger();
	
	bool SecondaryGetButtonA();
	bool SecondaryGetButtonB();
	bool SecondaryGetButtonX();
	bool SecondaryGetButtonY();
	
	bool SecondaryGetButtonBack();
	bool SecondaryGetButtonStart();
	
	bool SecondaryGetLeftPushDown();
	bool SecondaryGetRightPushDown();
	
private:
	Joystick *primaryController, *secondaryController;
	CSVReader* csvReader;	
};

#endif 
