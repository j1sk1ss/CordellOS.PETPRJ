#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include "debug.h"

#include <hal/hal.h>
#include <arch/i686/irq.h>

extern uint8_t __bss_start;
extern uint8_t __end;

extern void _init();

void timer(Registers* regs) {
    printf(".");
}

void __attribute__((section(".entry"))) start(uint16_t bootDrive) {
    memset(&__bss_start, 0, (&__end) - (&__bss_start));

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