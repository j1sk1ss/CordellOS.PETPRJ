#pragma once

#include "pic.h"
#include "i8259.h"
#include "x86.h"
#include "stdio.h"
#include "isr.h"

typedef void (*IRQHandler)(Registers* regs);

void i386_irq_initialize();
void i386_irq_registerHandler(int irq, IRQHandler handler);