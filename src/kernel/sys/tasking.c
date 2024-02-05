#include "../include/tasking.h"

// TODO: make process that clean another processes

TaskManager* taskManager;

void TASK_task_init() {
	taskManager = (TaskManager*)malloc(sizeof(TaskManager));

    taskManager->tasksCount  = 0;
    taskManager->currentTask = -1;
}

void TASK_start_tasking() {
	i686_disableInterrupts();

	// Load stack to esp
	asm ("mov %%eax, %%esp": :"a"(taskManager->tasks[0]->cpuState->esp));

	// Set values from stack
	asm ("pop %gs");
	asm ("pop %fs");
	asm ("pop %es");
	asm ("pop %ds");
	asm ("pop %ebp");
	asm ("pop %edi");
	asm ("pop %esi");
	asm ("pop %edx");
	asm ("pop %ecx");
	asm ("pop %ebx");
	asm ("pop %eax");
	
	// Set multitasking on
	taskManager->currentTask = 0;

	i686_enableInterrupts();

	// Return from interrupt and execute process
	asm ("iret");
}

void TASK_stop_tasking() {
	taskManager->currentTask = -1;
}

Task* TASK_create_task(char* pname, uint32_t address) {

	//=============================
	// Allocate memory for new task

		Task* task              = (Task*)malloc(sizeof(Task));
		task->cpuState          = (CPUState*)malloc(sizeof(CPUState));
		task->cpuState->esp     = (uint32_t)malloc(PAGE_SIZE);
		uint32_t* stack_pointer = (uint32_t*)(task->cpuState->esp + PAGE_SIZE);

		task->state = PROCESS_STATE_ALIVE;
		task->pid   = -1;
		task->name  = pname;

		for (int pid = 0; pid < TASKS_MAX; pid++) {
			for (int id = 0; id < TASKS_MAX; id++) 
				if (taskManager->tasks[pid]->pid != id) {
					task->pid = id;
					break;
				}

			if (task->pid != -1) break;
		}

	// Allocate memory for new task
	//=============================

	//=============================
	// Fill EBP stack

		*--stack_pointer = 0x00000202; // eflags
		*--stack_pointer = 0x8; // cs
		*--stack_pointer = (uint32_t)address; // eip

		*--stack_pointer = 0; // eax
		*--stack_pointer = 0; // ebx
		*--stack_pointer = 0; // ecx
		*--stack_pointer = 0; // edx
		*--stack_pointer = 0; // esi
		*--stack_pointer = 0; // edi

		*--stack_pointer = task->cpuState->esp + PAGE_SIZE; //ebp

		*--stack_pointer = 0x10; // ds
		*--stack_pointer = 0x10; // fs
		*--stack_pointer = 0x10; // es
		*--stack_pointer = 0x10; // gs

	// Fill EBP stack
	//=============================

	//=============================
	// Fill registers

		task->cpuState->eflag = 0x00000202; // eflags
		task->cpuState->cs    = 0x8; // cs
		task->cpuState->eip   = (uint32_t)address; // eip

		task->cpuState->eax = 0; // eax
		task->cpuState->ebx = 0; // ebx
		task->cpuState->ecx = 0; // ecx
		task->cpuState->edx = 0; // edx
		task->cpuState->esi = 0; // esi
		task->cpuState->edi = 0; // edi

		task->cpuState->ebp = task->cpuState->esp + PAGE_SIZE; //ebp
		task->cpuState->esp = (uint32_t)stack_pointer;

	// Fill registers
	//==============================

    return task;
}

void destroy_task(Task* task) {
	free(task->cpuState->esp);
	free(task->cpuState);
    free(task);
}

Task* get_task(int pid) {
	for (int i = 0; i < TASKS_MAX; i++) 
		if (taskManager->tasks[i]->pid == pid) return taskManager->tasks[i];

	return NULL;
}

void __kill() {
	int current = taskManager->currentTask;
	taskManager->currentTask = -1;
	_kill(taskManager->tasks[current]->pid);

	taskManager->currentTask = current;
}

void _kill(int pid) {
    if (pid >= 0 && pid < TASKS_MAX) 
        for (int task = 0; task < TASKS_MAX; task++) 
			if (taskManager->tasks[task]->pid == pid) {
				taskManager->tasks[task]->state = PROCESS_STATE_DEAD;
				break;
			}
}

int _TASK_add_task(Task* task) {
    if (taskManager->tasksCount >= 256) return -1;

    taskManager->tasks[taskManager->tasksCount++] = task;
    return task->pid;
}

int TASK_add_task(Task* task) {
	int current = taskManager->currentTask;
	taskManager->currentTask = -1;

	_TASK_add_task(task);

	taskManager->currentTask = current;
	return task->pid;
}

CPUState* TASK_task_switch(CPUState* state) {
	i686_disableInterrupts();

	taskManager->tasks[taskManager->currentTask]->cpuState = state;

	// Change task
	if (++taskManager->currentTask >= taskManager->tasksCount)
			taskManager->currentTask = 0;

	Task* new_task = taskManager->tasks[taskManager->currentTask];
	while (new_task->state != PROCESS_STATE_ALIVE) {
		if (++taskManager->currentTask >= taskManager->tasksCount)
			taskManager->currentTask = 0;

		new_task = taskManager->tasks[taskManager->currentTask];
	}

	i686_enableInterrupts();
	return new_task->cpuState;
}