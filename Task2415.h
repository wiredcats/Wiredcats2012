#ifndef TASK2415_H_
#define TASK2415_H_

#include "WPILib.h"
#include "Defines.h"

#define TASK2415_DEFAULT_PRIORITY (200)
#define TASK2415_STACKSIZE (1024 * 64)
#define TASK2415_MAX_TASKS (32)

class Task2415 {
private:
	static Task2415 *TaskList[TASK2415_MAX_TASKS + 1];
	static int TasksListed;
	static int TasksSwapped;
	static bool FirstConstructor;

protected:
	int taskID;
	int taskState;
	TASK_STATUS taskStatus;
	char * taskName;

	bool isAwake;
	bool keepTaskAlive;
	bool taskStateComplete;
	
	double PIDSpecific;

public:
	Task2415(void);
	
	void Start(char * id);
	static int TaskWrapper(void *this_p, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
	virtual int Main(int a2, int a3, int a4, int a5, int a6,  int a7, int a8, int a9, int a10) = 0;
	
	static Task2415 *SearchForTask(char * query);

	static void SetTaskStatuses(TASK_STATUS ts);
	void SetIndividualStatus(TASK_STATUS ts);

	void SetState(int state);
	void SetPIDSpecific(double pwm);
	
	bool StateComplete();

	char * GetTaskName();

	void SwapAndWait();
	void KillTask();
};

#endif
