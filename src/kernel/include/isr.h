#ifndef ISR_H
#define ISR_H


#include <stdint.h>
#include <stdbool.h>

#include "pit.h"
#include "idt.h"
#include "gdt.h"
#include "x86.h"
#include "stdio.h"

#include "../util/arrays.h"


typedef struct RegsStruct {
    uint32_t ds;                                            // data segment pushed by us
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // we push interrupt and error code
    uint32_t eip, cs, eflag, esp, ss;                       // pushed auto by cpu
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);


void i386_isr_initialize();
void i386_isr_registerHandler(int interrupt, ISRHandler handler);

#endif
