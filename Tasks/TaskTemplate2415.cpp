#include "TaskTemplate2415.h"

TaskTemplate2415::TaskTemplate2415(void) {
	global = new Global();

	Start("template2415");
}

int TaskTemplate2415::Main(int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	printf("entering %s main", taskName);
	while (keepTaskAlive) {
		if (taskStatus == STATUS_TELEOP) {
			// teleop schmazz
		}
		SwapAndWait();
	}

	return 0;
}

