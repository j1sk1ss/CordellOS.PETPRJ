#ifndef TASKING_H
#define TASKING_H

#include <stdint.h>

#include "x86.h"
#include "stdio.h"
#include "memory.h"
#include "allocator.h"

#define TASKS_MAX             256

#define PROCESS_STATE_ALIVE   0 
#define PROCESS_STATE_STRANGE 1
#define PROCESS_STATE_DEAD    2

#define START_PROCESS(name, address)    TASK_add_task(TASK_create_task(name, address))

typedef struct {
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflag, esp;
} __attribute__((packed)) CPUState;

typedef struct {
    CPUState* cpuState;

    char* name;
    int state;
    int pid;

} Task;

typedef struct {
    Task* tasks[TASKS_MAX];

    int tasksCount;
    int currentTask;

} TaskManager;


extern TaskManager* taskManager;


void TASK_task_init();
void TASK_start_tasking();

Task* TASK_create_task(char* pname, uint32_t address);
int TASK_add_task(Task* task);

CPUState* TASK_task_switch(CPUState* state);

void __kill();
void _kill(int pid);

#endif // TASKING_H