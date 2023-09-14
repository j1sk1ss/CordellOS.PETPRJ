#include "stdio.h"
#include "memory.h"

#include <stdint.h>
#include <boot/bootparams.h>
#include <debug.h>
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/io.h>


extern void _init();

void timer(Registers* regs) {
    printf(".");
}

void start(BootParams* bootParams) {

    // global constructors
    _init();

    HAL_initialize();

    printf("Cordell OS v0.a\n");
    printf("This operating system is under construction.\n");

    log_debug("Main.c", "Boot device! %x", bootParams->BootDevice);
    log_cordell("Main.c", "This is a special message from OS!");

end:
    for (;;);
}