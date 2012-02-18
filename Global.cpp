#include "Global.h"

//TODO Rewrite this with the same enum buttons as CheesyPoofs

Global *Global::myself = 0;

Global::Global(void) {
	static bool alreadyRun = false;
	
	if (!alreadyRun) {
		xboxController = new Joystick(1);		
		csvReader = new CSVReader("CheesyConfig.csv"); //Note that this is location on cRIO
		myself = this;

		alreadyRun = true;
	}

	printf("instantiated global\n"); 
}

double Global::GetLeftX() { return xboxController->GetRawAxis(1); }
double Global::GetLeftY() { return xboxController->GetRawAxis(2); } 
double Global::GetRightX() {return xboxController->GetRawAxis(4); }
double Global::GetRightY() { return xboxController->GetRawAxis(5); }
double Global::GetDPadX() { return xboxController->GetRawAxis(6); }

bool Global::GetLeftTrigger2() { return (xboxController->GetRawAxis(3) > 0); }
bool Global::GetRightTrigger2() { return (xboxController->GetRawAxis(3) < 0); }

bool Global::GetButtonA() {return xboxController->GetRawButton(1); }
bool Global::GetButtonB() {return xboxController->GetRawButton(2); }
bool Global::GetButtonX() {return xboxController->GetRawButton(3); }
bool Global::GetButtonY() {return xboxController->GetRawButton(4); }
bool Global::GetLeftTrigger1() {return (xboxController->GetRawButton(5)); }
bool Global::GetRightTrigger1() { return(xboxController->GetRawButton(6)); }
bool Global::GetButtonBack() { return xboxController->GetRawButton(7); }
bool Global::GetButtonStart() {return xboxController->GetRawButton(8); }
bool Global::GetLeftPushDown() {return xboxController->GetRawButton(9); }
bool Global::GetRightPushDown() {return xboxController->GetRawButton(10); }

double Global::ReadCSV(const std::string& valueName) { return(csvReader->GetValue(valueName)); }
void Global::ResetCSV() { csvReader->ReloadValues(); }

double Global::LinearizeVictor(double goal_speed) {
	const double deadband_value = 0.082;
	
	if (goal_speed > deadband_value){
		goal_speed -= deadband_value;
		} else if (goal_speed < -deadband_value) {
			goal_speed += deadband_value;
			} else {
				goal_speed = 0.0;		
				}

	goal_speed = goal_speed / (1.0 - deadband_value);

	double goal_speed2 = goal_speed * goal_speed;
	double goal_speed3 = goal_speed2 * goal_speed;
	double goal_speed4 = goal_speed3 * goal_speed;
	double goal_speed5 = goal_speed4 * goal_speed;
	double goal_speed6 = goal_speed5 * goal_speed;
	double goal_speed7 = goal_speed6 * goal_speed;

	// Constants for the 5th order polynomial
	double victor_fit_e1 = 0.437239;
	double victor_fit_c1 = -1.56847;
	double victor_fit_a1 = (- (125.0 * victor_fit_e1  + 125.0 * victor_fit_c1 - 116.0) / 125.0);
	double answer_5th_order = (victor_fit_a1 * goal_speed5
					+ victor_fit_c1 * goal_speed3
					+ victor_fit_e1 * goal_speed);

	// Constants for the 7th order polynomial
	double victor_fit_c2 = -5.46889;
	double victor_fit_e2 = 2.24214;
	double victor_fit_g2 = -0.042375;
	double victor_fit_a2 = (- (125.0 * (victor_fit_c2 + victor_fit_e2 + victor_fit_g2) - 116.0) / 125.0);
	double answer_7th_order = (victor_fit_a2 * goal_speed7
					+ victor_fit_c2 * goal_speed5
					+ victor_fit_e2 * goal_speed3
					+ victor_fit_g2 * goal_speed);


	// Average the 5th and 7th order polynomials
	double answer =  0.85 * 0.5 * (answer_7th_order + answer_5th_order)
			+ .15 * goal_speed * (1.0 - deadband_value);

	if (answer > 0.001)
		answer += deadband_value;
	else if (answer < -0.001)
		answer -= deadband_value;

	return answer;
}

Global* Global::GetInstance() {
	return myself;
}

