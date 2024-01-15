#pragma once

#include <stdint.h>

#define SYS_PRINT            0
#define SYS_PUTC             1
#define SYS_CLEAR            2
#define SYS_SLEEP            3 
#define SYS_READ_KEYBOARD    4
#define SYS_GET_KEY_KEYBOARD 5
#define SYS_READ_FILE        6  // Not implemented yet
#define SYS_WRITE_FILE       7  // Not implemented yet

typedef struct {
    uint32_t ds;                                            // data segment pushed by us
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // we push interrupt and error code
    uint32_t eip, cs, eflag, esp, ss;                       // ushed auto by cpu
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i686_isr_initialize();
void i686_isr_registerHandler(int interrupt, ISRHandler handler);
