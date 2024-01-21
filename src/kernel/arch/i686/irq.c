#include "../../include/irq.h"
#include "../../include/pic.h"
#include "../../include/i8259.h"
#include "../../include/x86.h"
#include "../../include/stdio.h"

#include <util/arrays.h>
#include <stddef.h>

#define PIC_REMAP_OFFSET 0x20

IRQHandler _handler[16];
static const PICDriver* _PICDriver = NULL; 

void i686_irq_handler(Registers* regs) {
    int irq = regs->interrupt - PIC_REMAP_OFFSET;
    
    uint8_t pic_isr = i8259_readIRQInServiceRegisters();
    uint8_t pic_irr = i8259_readIRQRequestRegisters();

    // send EOI
    _PICDriver->SendEndOfInterrupt(irq);
}

void i686_irq_initialize() {
    const PICDriver* drivers[] = {
        i8259_getDriver(),
    };

    for (int i = 0; i < SIZE(drivers); i++) 
        if (drivers[i]->Probe()) _PICDriver = drivers[i];

    if (_PICDriver == NULL) {
        kprintf("Cordell Warning: NO PIC!\n");
        return;
    }

    kprintf("PIC %s finded!\n", _PICDriver->Name);
    _PICDriver->Initialize(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, false);

    // Reg ISR handlers
    for (int i = 0; i < 16; i++)
        i686_isr_registerHandler(PIC_REMAP_OFFSET + i, i686_irq_handler);

    // Enable interrups
    i686_enableInterrupts();

    _PICDriver->Unmask(0);
    _PICDriver->Unmask(1);
}

void i686_irq_registerHandler(int irq, IRQHandler handler) {
    _handler[irq] = handler;
}