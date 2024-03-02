#include "../include/hal.h"

void HAL_initialize() {
    i386_gdt_initialize();
    kprintf("GDT initialized\n");

    i386_idt_initialize();
    kprintf("IDT initialized\n");

    i386_isr_initialize();
    kprintf("ISR initialized\n");

    i386_irq_initialize();
    kprintf("IRQ initialized\n");

    TSS_init(5, 0x10, 0);
    kprintf("TSS initialized\n");
}
