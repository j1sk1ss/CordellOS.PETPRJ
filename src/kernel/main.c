#include <stdint.h>

#include <boot/bootparams.h>

#include "memory/memory.h"
#include "memory/allocator/malloc.h"

#include <io/debug.h>
#include "io/string.h"
#include "io/stdio.h"

#include <hal/hal.h>

#include <arch/i686/irq.h>
#include <arch/i686/io.h>

#include "Keyboard.h"


extern void _init();

void start(BootParams* bootParams) {
    _init();                            // global constructors
    mm_init(0x50000);                   // Gernel Load is 0x50000 and kernel size is 0x00010000. Malloc start in 0x50000
    HAL_initialize();
    x86_init_keyboard();

    printf("Cordell OS v0.a\r\n");
    printf("Questo operato system e un sopra constructione.\r\n");

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);
    
    while (1) {
        printf("\r\n[CORDELL OS]: ");

        char* command = keyboard_read(1);
        if (strstr(command, "help") == 0)              
            help_command();
        else if (strstr(command, "cordell") == 0) {
            char* cordellCommand = command + strlen("cordell") + 1; 

            printf("\r\n[PASSWORD]: ");
            char* password = keyboard_read(0);
            while (strcmp(password, "12345") != 0) {
                printf("\r\nIncorrect password, try again.\r\n[PASSWORD]: ");
                free(password);

                password = keyboard_read(0);
            }

            if (strstr(cordellCommand, "hello") == 0)
                shut_down_command();
        } else 
            printf("\r\nUnknown command. Maybe you forgot CORDELL?");
            

        free(command);
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
    printf("\r\n> Shut-down test");
    printf("\r\n> Test line");
}