#pragma once

#include <stdint.h>

#define SYS_PRINT       0
#define SYS_READ_FILE   1
#define SYS_WRITE_FILE  2

typedef struct {
    uint32_t ds;                                            // data segment pushed by us
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // we push interrupt and error code
    uint32_t eip, cs, eflag, esp, ss;                       // ushed auto by cpu
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i686_isr_initialize();
void i686_isr_registerHandler(int interrupt, ISRHandler handler);
