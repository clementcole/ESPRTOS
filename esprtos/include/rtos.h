//RTOS Core
#ifndef _RTOS_H_
#define _RTOS_H_

#include "task.h"

#define MAX_TASKS 8 
#define IDLE_TASK_STACK_SIZE 256
#define MAIN_TASK_STACK_SIZE 1024 


//System Calls
void init(void);
void start(void);
void tick(void);


typedef void (*TaskFunction)(void);

struct TCB{
    TaskFunction task; 
    int priority;
    struct TCB *next;
};

void createTask(TaskFunction func, int priority);
void runScheduler();

#endif 

