#include "io/stdio.h"
#include "memory/memory.h"

#include <stdint.h>
#include <boot/bootparams.h>
#include <io/debug.h>
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <arch/i686/io.h>

#include "Keyboard.h"
#include "io/string.h"

extern void _init();

void start(BootParams* bootParams) {
    // global constructors
    _init();

    HAL_initialize();

    x86_init_keyboard();

    printf("Cordell OS v0.a\r\n");
    printf("This operating system is under construction.\r\n");

    log_debug("Main.c", "Boot device! %x", bootParams->BootDevice);
    log_cordell("Main.c", "This is a special message from OS!");

    char command[30];

    while (1) {
        printf("\r\n[CORDELL OS]: ");
        
        command[0] = '\0';
        keyboard_read(command);

        if (strcmp(command, "help") == 0)                   // Not correctly work
            printf("\r\n> Go fuck urself retard");
        else if (strcmp(command, "hello") == 0) 
            printf("\r\n> Please, go fuck urself retard");
    }

end:
    for (;;);
}