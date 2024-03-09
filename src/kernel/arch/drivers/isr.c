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

void i386_ISR_InitializeGates();

void i386_isr_initialize() {
    i386_ISR_InitializeGates();
    for (int i = 0; i < 256; i++)
        i386_idt_enableGate(i);
}

void __attribute__((cdecl)) i386_isr_handler(Registers* regs) {
    if (regs->interrupt < 256) {
        if (_isrHandlers[regs->interrupt] != NULL) {
            _isrHandlers[regs->interrupt](regs);
            return;
        }

        if (regs->interrupt < SIZE(_exceptions) && _exceptions[regs->interrupt] != NULL) 
            kprintf("Unhandled exception %d %s\n", regs->interrupt, _exceptions[regs->interrupt]);
        else  kprintf("Unhandled interrupt! Interrupt: %d\n", regs->interrupt);
        
        kprintf("  eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x\n",
                regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
        kprintf("  esp=%08x ebp=%08x eip=%08x eflags=%08x cs=%08x ds=%08x ss=%08x\n",
                regs->esp, regs->ebp, regs->eip, regs->eflag, regs->cs, regs->ds, regs->ss);
        kprintf("  interrupt=%08x errorcode=%08x\n", regs->interrupt, regs->error);

        kernel_panic("\nKERNEL PANIC\n");
    }
}


void i386_isr_registerHandler(int interrupt, ISRHandler handler) {
    _isrHandlers[interrupt] = handler;
    i386_idt_enableGate(interrupt);
}