#include <stdint.h>
#include <boot/bootparams.h>

#include "memory/memory.h"

#include <io/debug.h>
#include "io/string.h"
#include "io/stdio.h"

#include <hal/hal.h>

#include <arch/i686/irq.h>
#include <arch/i686/io.h>

#include "Keyboard.h"


extern void _init();

void start(BootParams* bootParams) {
    // global constructors
    _init();

    HAL_initialize();

    x86_init_keyboard();

    printf("Cordell OS v0.a\r\n");
    printf("Questo operato system e un sopra constructione.\r\n");

    log_debug("Main.c", "Boot device! %x", bootParams->BootDevice);
    log_cordell("Main.c", "This is a special message from OS!");
    
    while (1) {
        printf("\r\n[CORDELL OS]: ");

        char* command = keyboard_read();
        
        if (strcmp(command, "help") == 0)                   // Not correctly work
            printf("\r\n> 1");
        else if (strcmp(command, "hello") == 0) 
            printf("\r\n> 2");
    }

end:
    for (;;);
}