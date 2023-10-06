#include <stdint.h>

#include <boot/bootparams.h>

#include <include/memory.h>
#include <include/debug.h>
#include <include/string.h>
#include <include/stdio.h>
#include <include/hal.h>
#include <include/irq.h>
#include <include/io.h>

#include "shell/shell.h"

extern void _init();

void start(BootParams* bootParams) {
    _init();                            // global constructors
    mm_init(0x50000);                   // Kernel loads in 0x40000 and kernel size is 0x00010000. Malloc start in 0x50000 + 0x1000
    HAL_initialize();
    x86_init_keyboard();

    shell();

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);
    
end:
    for (;;);
}