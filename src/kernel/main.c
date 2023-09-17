#include <stdint.h>

#include <boot/bootparams.h>

#include "memory/memory.h"
#include "memory/allocator/allocator.h"
#include "memory/allocator/string_allocator.h"

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
    initializeMemoryPool();
    HAL_initialize();
    x86_init_keyboard();

    printf("Cordell OS v0.a\r\n");
    printf("Questo operato system e un sopra constructione.\r\n");

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);
    
    while (1) {
        printf("\r\n[CORDELL OS]: ");

        char* command = keyboard_read();
        if (strcmp(command, "help") == 0)                   // Not correctly work
            help_command();
        else if (strcmp(command, "shut-down") == 0) 
            shut_down_command();

        free_string(command);
    }

end:
    for (;;);
}

// Help command
void help_command() {
    printf("\r\n> Use help for getting help");
    printf("\r\n> Use shut-down for shut down");
    printf("\r\n> Test line");
    printf("\r\n> Test line");
    printf("\r\n> Test line");
}

// Shut down command
void shut_down_command() {

}