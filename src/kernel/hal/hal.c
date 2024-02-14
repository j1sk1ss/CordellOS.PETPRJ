#include "../include/hal.h"

#include <include/gdt.h>
#include <include/idt.h>
#include <include/isr.h>
#include <include/irq.h>

#include <include/vga_text.h>

void HAL_initialize() {
    i386_gdt_initialize();
    i386_idt_initialize();
    i386_isr_initialize();
    i386_irq_initialize();
}
