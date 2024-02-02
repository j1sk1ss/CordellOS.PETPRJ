#include "../include/hal.h"

#include <include/gdt.h>
#include <include/idt.h>
#include <include/isr.h>
#include <include/irq.h>

#include <include/vga_text.h>

void HAL_initialize() {
    i686_gdt_initialize();
    i686_idt_initialize();
    i686_isr_initialize();
    i686_irq_initialize();
}
