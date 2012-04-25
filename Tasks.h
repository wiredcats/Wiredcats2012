#include "Tasks/Drive2415.h"
#include "Tasks/Turret2415.h"
#include "Tasks/Intake2415.h"

#include "Controllers/AutoBalance.h"
#include "Controllers/AutoTracker.h"
#include "Controllers/ShootController.h"

#include "Controllers/FeedAutonomous.h"
#include "Controllers/BridgeAutonomous.h"

//Tasks
Drive2415 drive; 
Turret2415 turret;
Intake2415 intake;

//Controllers
AutoBalance2415 autobalance;
AutoTracker2415 autotracker;
ShootController2415 shoot;

//Autonomous
FeedAutonomous2415 autonomous;
//BridgeAutonomous2415 autonomous;
