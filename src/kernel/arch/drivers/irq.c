#include "../../include/irq.h"
#include "../../util/arrays.h"

#include <stddef.h>


#define PIC_REMAP_OFFSET 0x20


IRQHandler _handler[16] = { NULL };
static const PICDriver* _PICDriver = NULL; 


void i386_irq_handler(Registers* regs) {
    int irq = regs->interrupt - PIC_REMAP_OFFSET;
    uint8_t pic_isr = i8259_readIRQInServiceRegisters();
    uint8_t pic_irr = i8259_readIRQRequestRegisters();
    
    if (_handler[irq] != NULL) _handler[irq](regs);
    else kprintf("[%s %i] No handler for: %i\n", __FILE__, __LINE__, irq);

    _PICDriver->SendEndOfInterrupt(irq);
}

void i386_irq_initialize() {
    const PICDriver* drivers[] = { i8259_getDriver(), };

    for (int i = 0; i < SIZE(drivers); i++) 
        if (drivers[i]->Probe()) _PICDriver = drivers[i];

    if (_PICDriver == NULL) {
        kprintf("Cordell Warning: NO PIC!\n");
        return;
    }
    
    kprintf("PIC %s finded!\n", _PICDriver->Name);
    _PICDriver->Initialize(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 12, false);

    for (int i = 0; i < 16; i++)
        i386_isr_registerHandler(PIC_REMAP_OFFSET + i, i386_irq_handler);

    i386_enableInterrupts();

    _PICDriver->Unmask(2); // slave interrupt controller allowing for IRQ 8-15
}

void i386_irq_registerHandler(int irq, IRQHandler handler) {
    _handler[irq] = handler;
    _PICDriver->Unmask(irq);
}