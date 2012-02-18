#include "Turret2415.h"

Turret2415::Turret2415(void) {
	global = Global::GetInstance();

	jagTurret = new Jaguar(3);

	taskState = WAIT_FOR_INPUT;

	Start("turret2415");
}

int Turret2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main\n", taskName);
	while (keepTaskAlive) {
		if (taskStatus == STATUS_AUTO || taskStatus == STATUS_TELEOP) {
			switch (taskState) {
				case WAIT_FOR_INPUT:
					jagTurret->Set(0.0);
					break;
				case MOVE_LEFT:
					jagTurret->Set(-(global->ReadCSV("TURRET_SPEED")));
					break;
				case MOVE_RIGHT:
					jagTurret->Set(global->ReadCSV("TURRET_SPEED"));
					break;
				default:
					jagTurret->Set(0.0);
					break;
			}
		}
		SwapAndWait();
	}

	return 0;
}

