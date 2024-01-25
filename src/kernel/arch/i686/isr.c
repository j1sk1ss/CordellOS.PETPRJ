#include "../../include/isr.h"

#include<stddef.h>

ISRHandler _isrHandlers[256];

static const char* const _exceptions[] = {
    "Divide by zero Cordell-error",           "Cordell-Debug",
    "Non-maskable Cordell-Interrupt",         "Cordell-Breakpoint",
    "Cordell-Overflow",                       "Cordell-Bound Range Exceeded",
    "Invalid Cordell-Opcode",                 "Cordell-Device Not Available",
    "Double Cordell-Fault",                   "Coprocessor Cordell-Segment Overrun",
    "Invalid Cordell-TSS",                    "Cordell-Segment Not Present",
    "Cordell-Stack-Segment Fault",            "General Cordell-Protection Fault",
    "Cordell-Page Fault", "",                 "x87 Floating-Point Cordell-Exception",
    "Cordell-Alignment Check",                "Cordell-Machine Check",
    "SIMD Floating-Point Cordell-Exception",  "Virtualization Cordell-Exception",
    "Control Protection Cordell-Exception ",  "", "", "", "", "", "",
    "Hypervisor Injection Cordell-Exception", "VMM Communication Cordell-Exception",
    "Security Cordell-Exception", ""
};

void i686_ISR_InitializeGates();

void i686_isr_initialize() {
    i686_ISR_InitializeGates();
    for (int i = 0; i < 256; i++)
        i686_idt_enableGate(i);

    i686_isr_registerHandler(0x80, syscall);
}

void __attribute__((cdecl)) i686_isr_handler(Registers* regs) {
    if (regs->interrupt == 32) 
        if (taskManager->tasksCount > 0 && taskManager->currentTask >= 0) { // Check if multitasking enabled

            //=====================
            // Create current state

                CPUState* current_state = (CPUState*)malloc(sizeof(CPUState));
                memset(current_state, 0, sizeof(CPUState));

                current_state->eflag = regs->eflag;
                current_state->cs    = regs->cs;
                current_state->eip   = regs->eip;
                current_state->eax   = regs->eax;
                current_state->ebx   = regs->ebx;
                current_state->ecx   = regs->ecx;
                current_state->edx   = regs->edx;
                current_state->esi   = regs->esi;
                current_state->edi   = regs->edi;
                current_state->ebp   = regs->ebp;

                current_state->esp   = regs->kern_esp;

            // Create current state
            //=====================
            
            //=====================
            // Update current state

                CPUState* new_state = TASK_task_switch(current_state);

                regs->eflag = new_state->eflag;
                regs->cs    = new_state->cs;
                regs->eip   = new_state->eip;
                regs->eax   = new_state->eax;
                regs->ebx   = new_state->ebx;
                regs->ecx   = new_state->ecx;
                regs->edx   = new_state->edx;
                regs->esi   = new_state->esi;
                regs->edi   = new_state->edi;
                regs->ebp   = new_state->ebp;

                regs->esp      = new_state->esp;
                regs->kern_esp = new_state->esp;

            // Update current state
            //=====================

            asm ("out %%al, %%dx": :"d"(0x20), "a"(0x20));
            return;
        };
    
    if (_isrHandlers[regs->interrupt] != NULL) _isrHandlers[regs->interrupt](regs);
    else if (regs->interrupt > 32) kprintf("Unhandled interrupt! Interrupt: %d\n", regs->interrupt);
    else {
        kprintf("Unhandled exception %d %s\n", regs->interrupt, _exceptions[regs->interrupt]);
        kprintf("  eax=%x  ebx=%x  ecx=%x  edx=%x  esi=%x  edi=%x\n",
               regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
        kprintf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
               regs->esp, regs->ebp, regs->eip, regs->eflag, regs->cs, regs->ds, regs->ss);
        kprintf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);
        kernel_panic("\nKERNEL PANIC\n");
    }
}

void i686_isr_registerHandler(int interrupt, ISRHandler handler) {
    _isrHandlers[interrupt] = handler;
    i686_idt_enableGate(interrupt);
}