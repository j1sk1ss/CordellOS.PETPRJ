#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include "debug.h"

#include <hal/hal.h>
#include <arch/i686/irq.h>


extern void _init();

void timer(Registers* regs) {
    printf(".");
}

void start(uint16_t bootDrive) {

    // global constructors
    _init();

    HAL_initialize();

    printf("Cordell OS v0.a\n");
    printf("This operating system is under construction.\n");

    log_debug("Main.c", "This is a debug message!");
    log_cordell("Main.c", "This is a special message from OS!");

end:
    for (;;);
}