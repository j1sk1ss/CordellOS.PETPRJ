#ifndef TASKING_H
#define TASKING_H


#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>

#include "x86.h"
#include "stdio.h"
#include "irq.h"


#define TASKS_MAX             256

#define PROCESS_STATE_ALIVE   0 
#define PROCESS_STATE_STRANGE 1
#define PROCESS_STATE_DEAD    2

#define START_PROCESS(name, address)    TASK_add_task(TASK_create_task(name, address))


typedef struct {
    Registers* cpuState;

    char* name;
    int state;
    int pid;

    uint32_t virtual_address;
    uint32_t page_directory;

} Task;

typedef struct {
    Task* tasks[TASKS_MAX];

    int tasksCount;
    int currentTask;

} TaskManager;


extern TaskManager* taskManager;
extern bool tasking;


void i386_task_init();

void TASK_start_tasking();
void TASK_stop_tasking();
void TASK_continue_tasking();

Task* TASK_create_task(char* pname, uint32_t address);
int TASK_add_task(Task* task);

void TASK_task_switch(Registers* state);

void __kill();
void _kill(int pid);


#endif // TASKING_H