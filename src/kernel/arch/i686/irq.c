#include "irq.h"
#include "pic.h"
#include "io.h"
#include "stdio.h"

#include <stddef.h>

#define PIC_REMAP_OFFSET 0x20

IRQHandler _handler[16];

void i686_irq_handler(Registers* regs) {
    int irq = regs->interrupt - PIC_REMAP_OFFSET;
    
    uint8_t pic_isr = i686_pic_readIRQInServiceRegisters();
    uint8_t pic_irr = i686_pic_readIRQRequestRegisters();

    if (_handler[irq] != NULL) {
        // handle IRQ
        _handler[irq](regs);
    }
    else {
        printf("Unhandled IRQ %d  ISR=%x  IRR=%x...\n", irq, pic_isr, pic_irr);
    }

    // send EOI
    i686_sendEndOfInterrupt(irq);
}

void i686_irq_initialize() {
    i686_pic_configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    // Reg ISR handlers
    for (int i = 0; i < 16; i++)
        i686_isr_registerHandler(PIC_REMAP_OFFSET + i, i686_irq_handler);

    // Enable interrups
    i686_enableInterrupts();
}

void i686_irq_registerHandler(int irq, IRQHandler handler) {
    _handler[irq] = handler;
}