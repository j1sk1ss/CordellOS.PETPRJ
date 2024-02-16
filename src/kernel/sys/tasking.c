#include "../include/tasking.h"

// TODO: make process that clean another processes

TaskManager* taskManager;


//==================
// Task interaction
//==================

	int task_position = -1;
	void TASK_stop_tasking() {
		task_position = taskManager->currentTask;
		taskManager->currentTask = -1;
	}

	void TASK_continue_tasking() {
		taskManager->currentTask = task_position;
	}

//==================
// Task interaction
//==================
// Task manager
//==================

	void TASK_task_init() {
		taskManager = malloc(sizeof(TaskManager));
		if (taskManager == NULL) {
			kprintf("Unnable to allocate memory!\n");
			kernel_panic("\nTASKING ERROR!");
		}

		taskManager->tasksCount = 0;
		TASK_stop_tasking();
	}

	void TASK_start_tasking() {
		i386_disableInterrupts();

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
		task_position = 0;

		i386_enableInterrupts();

		// Return from interrupt and execute process
		asm ("iret");
	}

	Task* TASK_create_task(char* pname, uint32_t address) {

		//=============================
		// Allocate memory for new task

			Task* task     = (Task*)malloc(sizeof(Task));
			task->cpuState = (CPUState*)malloc(sizeof(CPUState));

			//=============================
			// Find new pid

				task->state = PROCESS_STATE_ALIVE;
				task->pid   = -1;
				task->name  = pname;

				if (taskManager->tasksCount <= 0) task->pid = 0;
				for (int pid = 0; pid < taskManager->tasksCount; pid++) {
					for (int id = 0; id < TASKS_MAX; id++) 
						if (taskManager->tasks[pid]->pid != id) {
							task->pid = id;
							break;
						}

					if (task->pid != -1) break;
				}

				if (task->pid == -1) {
					free(task->cpuState);
					free(task);
				}

			// Find new pid
			//=============================
			// Allocate data for stack

				task->cpuState->esp     = (uint32_t)malloc(PAGE_SIZE);
				task->cpuState->virtual = task->cpuState->esp;
				uint32_t* stack_pointer = (uint32_t*)(task->cpuState->esp + PAGE_SIZE);

				asm ("mov %%cr3, %%eax":"=a"(task->cpuState->cr3));

			// Allocate data for stack
			//=============================

		// Allocate memory for new task
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
		// Fill registers

			task->cpuState->eflag = 0x00000202;
			task->cpuState->cs    = 0x8;
			task->cpuState->eip   = (uint32_t)address;

			task->cpuState->eax = 0;
			task->cpuState->ebx = 0;
			task->cpuState->ecx = 0;
			task->cpuState->edx = 0;
			task->cpuState->esi = 0;
			task->cpuState->edi = 0;

			task->cpuState->ebp = task->cpuState->esp + PAGE_SIZE;
			task->cpuState->esp = (uint32_t)stack_pointer;

		// Fill registers
		//==============================

		return task;
	}

	void destroy_task(Task* task) {
		free((uint32_t*)task->cpuState->virtual);
		free(task->cpuState);
		free(task);
	}

	Task* get_task(int pid) {
		for (int i = 0; i < TASKS_MAX; i++) 
			if (taskManager->tasks[i]->pid == pid) return taskManager->tasks[i];

		return NULL;
	}

	void __kill() { // TODO: complete multitask disabling when tasks == 0
		TASK_stop_tasking();
		_kill(taskManager->tasks[task_position]->pid);

		TASK_continue_tasking();
	}

	void _kill(int pid) {
		if (pid >= 0 && pid < taskManager->tasks) 
			for (int task = 0; task < taskManager->tasks; task++) 
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
		TASK_stop_tasking();

		_TASK_add_task(task);

		TASK_continue_tasking();
		return task->pid;
	}

	CPUState* TASK_task_switch(CPUState* state) { // TODO: Cpu state switchs with errors. Mem leak and pages
		i386_disableInterrupts();

		Task* task = taskManager->tasks[taskManager->currentTask];
		task->cpuState->eflag = state->eflag;
		task->cpuState->cs    = state->cs;
		task->cpuState->eip   = state->eip;
		task->cpuState->eax   = state->eax;
		task->cpuState->ebx   = state->ebx;
		task->cpuState->ecx   = state->ecx;
		task->cpuState->edx   = state->edx;
		task->cpuState->esi   = state->esi;
		task->cpuState->edi   = state->edi;
		task->cpuState->ebp   = state->ebp;

		task->cpuState->esp      = state->esp;
		task->cpuState->kern_esp = state->esp;

		free(state);

		if (++taskManager->currentTask >= taskManager->tasksCount)
				taskManager->currentTask = 0;

		Task* new_task = taskManager->tasks[taskManager->currentTask];
		while (new_task->state != PROCESS_STATE_ALIVE) {
			if (++taskManager->currentTask >= taskManager->tasksCount)
				taskManager->currentTask = 0;

			new_task = taskManager->tasks[taskManager->currentTask];
		}

		i386_enableInterrupts();
		return new_task->cpuState;
	}

//==================
// Task manager
//==================