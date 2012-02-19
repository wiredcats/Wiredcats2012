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
	
	Joystick* GetXboxController();

	//NOTE DO NOT HIT MODE SWITCH. THIS WILL CHANGE LEFT Y INTO DPAD Y	
	double GetLeftX();
	double GetLeftY();
	double GetRightX();
	double GetRightY();
	double GetDPadX();
	
	bool GetLeftTrigger2();
	bool GetRightTrigger2();
	
	bool GetButtonA();
	bool GetButtonB();
	bool GetButtonX();
	bool GetButtonY();
	bool GetLeftTrigger1();
	bool GetRightTrigger1();
	bool GetButtonBack();
	bool GetButtonStart();
	bool GetLeftPushDown();
	bool GetRightPushDown();
		
	double ReadCSV(const std::string& valueName);
	void ResetCSV();
	
	double LinearizeVictor(double goal_speed);
	
private:
	Joystick *xboxController;
	CSVReader* csvReader;	
};

#endif 
