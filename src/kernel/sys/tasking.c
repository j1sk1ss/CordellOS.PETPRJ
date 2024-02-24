#include "../include/tasking.h"

// TODO: make process that clean another processes
//		 v_addr allocation?

TaskManager* taskManager;
bool tasking = false;
uint32_t v_addr = 0x500000;


//==================
// Task interaction
//==================

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
		tasking = true;

		i386_enableInterrupts();

		// Return from interrupt and execute process
		asm ("iret");		
	}

	void TASK_stop_tasking() {
		tasking = false;
	}

	void TASK_continue_tasking() {
		tasking = true;
	}

//==================
// Task interaction
//==================
// Task manager
//==================

	void i386_task_init() {
		taskManager = calloc(sizeof(TaskManager), 1);
		if (taskManager == NULL) {
			kprintf("Unnable to allocate memory!\n");
			kernel_panic("\nTASKING ERROR!");
		}

		taskManager->tasksCount  = 0;
		taskManager->currentTask = -1;
		
		for (int i = 0; i < TASKS_MAX; i++)
			taskManager->tasks[i] = NULL;

		i386_irq_registerHandler(0, TASK_task_switch);
	}

	Task* TASK_create_task(char* pname, uint32_t address) {

		//=============================
		// Allocate memory for new task
		//=============================

			Task* task     = (Task*)calloc(sizeof(Task), 1);
			task->cpuState = (Registers*)calloc(sizeof(Registers), 1);

			//=============================
			// Find new pid
			//=============================

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

			//=============================
			// Find new pid
			//=============================
			// Allocate data for stack
			//=============================

				mallocp(v_addr);
				memset(v_addr, 0, PAGE_SIZE);

				task->cpuState->esp     = v_addr;
				task->virtual_address   = task->cpuState->esp;
				uint32_t* stack_pointer = (uint32_t*)(task->cpuState->esp + PAGE_SIZE);

				asm ("mov %%cr3, %%eax":"=a"(task->page_directory));

			//=============================
			// Allocate data for stack
			//=============================

		//=============================
		// Allocate memory for new task
		//=============================
		// Fill EBP stack
		//=============================

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

		//=============================
		// Fill EBP stack
		//=============================
		// Fill registers
		//=============================

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

		//=============================
		// Fill registers
		//==============================

		v_addr += PAGE_SIZE;

		return task;
	}

	void destroy_task(Task* task) {
		freep((uint32_t*)task->virtual_address);
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
		_kill(taskManager->tasks[taskManager->currentTask]->pid);
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

	void TASK_task_switch(Registers* regs) {
		if (tasking == false) return;
		
		Task* task = taskManager->tasks[taskManager->currentTask];
		if (task == NULL) return;

		task->cpuState->eflag = regs->eflag;
		task->cpuState->cs    = regs->cs;
		task->cpuState->eip   = regs->eip;
		task->cpuState->eax   = regs->eax;
		task->cpuState->ebx   = regs->ebx;
		task->cpuState->ecx   = regs->ecx;
		task->cpuState->edx   = regs->edx;
		task->cpuState->esi   = regs->esi;
		task->cpuState->edi   = regs->edi;
		task->cpuState->ebp   = regs->ebp;

		task->cpuState->esp      = regs->esp;
		task->cpuState->kern_esp = regs->esp;

		asm ("mov %%cr3, %%eax":"=a"(task->page_directory));

		if (++taskManager->currentTask >= taskManager->tasksCount)
				taskManager->currentTask = 0;

		Task* new_task = taskManager->tasks[taskManager->currentTask];
		while (new_task->state != PROCESS_STATE_ALIVE) {
			if (++taskManager->currentTask >= taskManager->tasksCount)
				taskManager->currentTask = 0;

			new_task = taskManager->tasks[taskManager->currentTask];
		}

		if (new_task == NULL) return;
		if (task->page_directory != new_task->page_directory)
			asm ("mov %%eax, %%cr3": :"a"(new_task->page_directory));

		regs->esp      = new_task->cpuState->esp;
		regs->kern_esp = new_task->cpuState->esp;

		regs->ebp   = new_task->cpuState->ebp;
		regs->edi   = new_task->cpuState->edi;
		regs->esi   = new_task->cpuState->esi;

		regs->edx   = new_task->cpuState->edx;
		regs->ecx   = new_task->cpuState->ecx;
		regs->ebx   = new_task->cpuState->ebx;
		regs->eax   = new_task->cpuState->eax;
		
		regs->eflag = new_task->cpuState->eflag;
		regs->cs    = new_task->cpuState->cs;
		regs->eip   = new_task->cpuState->eip;
	}

//==================
// Task manager
//==================