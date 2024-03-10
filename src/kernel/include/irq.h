#ifndef IRQ_H
#define IRQ_H


#include <stddef.h>

#include "isr.h"
#include "pic.h"
#include "i8259.h"
#include "x86.h"
#include "stdio.h"
#include "../util/arrays.h"


typedef void (*IRQHandler)(struct RegsStr* regs);


void i386_irq_initialize();
void i386_irq_registerHandler(int irq, IRQHandler handler);

#endif