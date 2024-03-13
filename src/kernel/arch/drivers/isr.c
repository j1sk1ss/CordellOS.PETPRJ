#include "../../include/isr.h"


ISRHandler _isrHandlers[256];

static const char* const _exceptions[] = {
    "DIVIDE BY ZERO",                 "DEBUG",
    "NON-MASKABLE INTERRUPT",         "BREAKPOINT",
    "OVERFLOW",                       "BOUND RANGE EXCEEDED",
    "INVALID OPCODE",                 "DEVICE NOT AVALIABLE",
    "DOUBLE FAULT",                   "COPROCESSOR SEGMENT OVERRUN",
    "INVALID TSS",                    "SEGMENT NOT PRESENT",
    "SS FAULT",                       "GENERAL PROTECTION FAULT",
    "PAGE FAULT", "",                 "X87 FLOATING-POINT EXCEPTION",
    "ALOGNMENT CHECK",                "MACHINE CHECK",
    "SIMD FLOACTING-POINT EXCEPTION", "VIRTUALIZATION EXCEPTION",
    "CONTROL PROTECTION EXCEPTION",   "", "", "", "", "", "",
    "HYPERVISOR INJECTION EXCEPTION", "VMM COMMUNICATION EXCEPTION",
    "SECURITY EXCEPTION", ""
};


void i386_ISR_InitializeGates();

void i386_isr_initialize() {
    i386_ISR_InitializeGates();
    for (int i = 0; i < 256; i++)
        i386_idt_enableGate(i);
}

void __attribute__((cdecl)) i386_isr_handler(struct Registers* regs) {
    if (regs->interrupt < 256) {
        if (_isrHandlers[regs->interrupt] != NULL) {
            _isrHandlers[regs->interrupt](regs);
            return;
        }

        if (regs->interrupt < SIZE(_exceptions) && _exceptions[regs->interrupt] != NULL) 
            kprintf("UNHANDLED EXCEPTION %d %s\n", regs->interrupt, _exceptions[regs->interrupt]);
        else  kprintf("UNHANDLED INTERRUPT! INTERRUPT: %d\n", regs->interrupt);
        
        kprintf("  eax=%u ebx=%u ecx=%u edx=%u esi=%u edi=%u\n",
                regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
        kprintf("  esp=%p ebp=%u eip=%p eflags=%u cs=%u ds=%u ss=%u\n",
                regs->esp, regs->ebp, regs->eip, regs->eflag, regs->cs, regs->ds, regs->ss);
        kprintf("  INTERRUPT=%u ERRORCODE=%u\n", regs->interrupt, regs->error);

        kernel_panic("\nKERNEL PANIC\n");
    }
}

void i386_isr_registerHandler(int interrupt, ISRHandler handler) {
    _isrHandlers[interrupt] = handler;
    i386_idt_enableGate(interrupt);
}