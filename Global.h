#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "WPILib.h"
#include "CSV/CSVReader.h"

#define GLOBAL_JLEFT_PORT	 (1)
#define GLOBAL_JRIGHT_PORT	 (2)
#define GLOBAL_JFAKEA_PORT	 (3)
#define GLOBAL_JFAKEB_PORT	 (4)

/* Global
 * ======
 * 
 * Variables that will be used across tasks are held here!
 * Global is in itself a task so that it constantly is fed the new values
 * This eliminates the need to constantly be passing copies of objects
 * around, thus quickening code!
 * 
 * Stuff located here:
 * 		- Joysticks
 * 		- Robot Status (disabled, operator, etc)
 * 		- Sensor Values (eventually)
 * 
 */

class Global {
private:
	// the instance
	static Global *myself;
	
	Joystick *xboxController;
	CSVReader* csvReader;

public:
	Global(void);

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

	// this is how tasks get access to the variables
	static Global *GetInstance();
};

#endif

