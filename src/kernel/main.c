#include <stdint.h>

#include <boot/bootparams.h>

#include <include/memory.h>
#include <include/debug.h>
#include <include/stdio.h>
#include <include/hal.h>
#include <include/irq.h>
#include <include/io.h>

extern void _init();

void start(BootParams* bootParams) {
    _init();                            // global constructors
    HAL_initialize();

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);
    
end:
    for (;;);
}