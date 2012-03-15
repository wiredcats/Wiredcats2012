#include "Global.h"

Global::Global() {
	primaryController = new Joystick(1);
	secondaryController = new Joystick(2);
	csvReader = new CSVReader("CheesyConfig.csv");   //Note that this is location on cRIO, not files		
}

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

//Primary Controller
double Global::PrimaryGetLeftX() { return primaryController->GetRawAxis(1); }
double Global::PrimaryGetLeftY() { return primaryController->GetRawAxis(2); } 
double Global::PrimaryGetRightX() {return primaryController->GetRawAxis(4); }
double Global::PrimaryGetRightY() { return primaryController->GetRawAxis(5); }
double Global::PrimaryGetDPadX() { return primaryController->GetRawAxis(6); }

bool Global::PrimaryGetLeftTrigger() { return (primaryController->GetRawAxis(3) > 0); }
bool Global::PrimaryGetRightTrigger() { return (primaryController->GetRawAxis(3) < 0); }

bool Global::PrimaryGetButtonA() {return primaryController->GetRawButton(1); }
bool Global::PrimaryGetButtonB() {return primaryController->GetRawButton(2); }
bool Global::PrimaryGetButtonX() {return primaryController->GetRawButton(3); }
bool Global::PrimaryGetButtonY() {return primaryController->GetRawButton(4); }
bool Global::PrimaryGetLeftBumper() {return (primaryController->GetRawButton(5)); }
bool Global::PrimaryGetRightBumper() { return(primaryController->GetRawButton(6)); }
bool Global::PrimaryGetButtonBack() { return primaryController->GetRawButton(7); }
bool Global::PrimaryGetButtonStart() {return primaryController->GetRawButton(8); }
bool Global::PrimaryGetLeftPushDown() {return primaryController->GetRawButton(9); }
bool Global::PrimaryGetRightPushDown() {return primaryController->GetRawButton(10); }

//Secondary Controller
double Global::SecondaryGetLeftX() { return secondaryController->GetRawAxis(1); }
double Global::SecondaryGetLeftY() { return secondaryController->GetRawAxis(2); } 
double Global::SecondaryGetRightX() {return secondaryController->GetRawAxis(4); }
double Global::SecondaryGetRightY() { return secondaryController->GetRawAxis(5); }
double Global::SecondaryGetDPadX() { return secondaryController->GetRawAxis(6); }

bool Global::SecondaryGetLeftTrigger() { return (secondaryController->GetRawAxis(3) > 0); }
bool Global::SecondaryGetRightTrigger() { return (secondaryController->GetRawAxis(3) < 0); }

bool Global::SecondaryGetButtonA() {return secondaryController->GetRawButton(1); }
bool Global::SecondaryGetButtonB() {return secondaryController->GetRawButton(2); }
bool Global::SecondaryGetButtonX() {return secondaryController->GetRawButton(3); }
bool Global::SecondaryGetButtonY() {return secondaryController->GetRawButton(4); }
bool Global::SecondaryGetLeftBumper() {return (secondaryController->GetRawButton(5)); }
bool Global::SecondaryGetRightBumper() { return(secondaryController->GetRawButton(6)); }
bool Global::SecondaryGetButtonBack() { return secondaryController->GetRawButton(7); }
bool Global::SecondaryGetButtonStart() {return secondaryController->GetRawButton(8); }
bool Global::SecondaryGetLeftPushDown() {return secondaryController->GetRawButton(9); }
bool Global::SecondaryGetRightPushDown() {return secondaryController->GetRawButton(10); }
