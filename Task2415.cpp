#include "Task2415.h"

Task2415::Task2415(void) {
	taskID = 0;
	taskState = 0;
	taskStatus = STATUS_BLANK;
	taskName = NULL;

	taskStateComplete = false;
	
	PIDSpecific = 0.0;

	isAwake = false;
	keepTaskAlive = true;

}

void Task2415::Start(char * id) {
	taskName = id;
	taskID = taskSpawn(id, TASK2415_DEFAULT_PRIORITY, VX_FP_TASK, TASK2415_STACKSIZE,
			   (FUNCPTR) Task2415::TaskWrapper, (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

int Task2415::TaskWrapper(void *this_p, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) {
	int j;
	for (j = 0; j < TASK2415_MAX_TASKS; j++) 
		if (TaskList[j] == NULL) break;

	TaskList[j] = (Task2415 *)this_p;
	TasksListed++;	

	printf("%s added to tasklist at %d\n", TaskList[j]->GetTaskName(), TasksListed);

	return ((Task2415 *)this_p)->Main(a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

Task2415* Task2415::SearchForTask(char * query) {
	int j;
	for (j = 0; j < TasksListed; j++) {
		printf("comparing query %s to task %s\n", query, TaskList[j]->GetTaskName());
		if (strcmp(TaskList[j]->GetTaskName(), query) == 0) {
			printf("found task %s\n", TaskList[j]->GetTaskName());
			 return TaskList[j];
		}
	}

	printf("ERROR: could not find task, exiting");

	return 0;
}

void Task2415::SetTaskStatuses(TASK_STATUS ts) {
	int j;
	for (j = 0; j < TASK2415_MAX_TASKS; j++) {
		if (TaskList[j] == NULL) break;
		TaskList[j]->SetIndividualStatus(ts);
	}
}

void Task2415::SetIndividualStatus(TASK_STATUS ts) { taskStatus = ts; }

void Task2415::SetState(int state) { 
	printf("changing %s state to %d\n", taskName, state);
	taskState = state;
	taskStateComplete = false;
}

int Task2415::GetState(void) {	return(taskState);	}

void Task2415::SetPWMSpecific(double pwm) {
	if (pwm > 1.0) pwm = 1.0;
	if (pwm < -1.0) pwm = -1.0;
	PIDSpecific = pwm;
}

bool Task2415::StateComplete() {
	return taskStateComplete;
}

char * Task2415::GetTaskName() { return taskName; }

void Task2415::SwapAndWait(void) {
	TasksSwapped++;

	taskUnlock();
	taskDelay(2);
	taskLock();

	if (TasksSwapped % 2 == 0) {
		Wait(STALL_LOOP_TIME);
	}
}

void Task2415::KillTask() { keepTaskAlive = false; }

