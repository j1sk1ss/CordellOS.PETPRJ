#include <stdint.h>

#include <boot/bootparams.h>

#include "memory/memory.h"

#include "../libs/core/shared/allocator/allocator.h"
#include "../libs/core/shared/file_system/ata.h"
#include "../libs/core/shared/file_system/file_system.h"
#include "../libs/core/shared/file_system/temp_file_system.h"

#include <io/debug.h>
#include "io/string.h"
#include "io/stdio.h"

#include <hal/hal.h>

#include <arch/i686/irq.h>
#include <arch/i686/io.h>

#include "calculator/calculator.h"

#include "Keyboard.h"

extern void _init();

char* currentPassword;

void start(BootParams* bootParams) {
    _init();                            // global constructors
    mm_init(0x50000);                   // Kernel Load is 0x50000 and kernel size is 0x00010000. Malloc start in 0x50000
    HAL_initialize();
    x86_init_keyboard();

    init_directory();
    // heap_init();
    // filesystem_init();

    printf("  _____  ____  ____   ___    ___ ||    ||        ____    ____\r\n");
    printf("_|      ||  || || ||  || || ||   ||    ||       ||  ||  |    \r\n");
    printf("||      ||  || ||_||  || || ||   ||    ||       ||  || ||    \r\n");
    printf("||      ||  || |||    || || ||__ ||    ||       ||  || ||    \r\n");
    printf("||      ||  || || ||  || || ||   ||    ||       ||  || ||    \r\n");
    printf(" |_____  |__|  ||  || ||_|| ||__ ||___ ||___     |__|   |____\r\n");

    printf("\r\n Questo sistema operativo 'e in costruzione. \r\n");

    // char *data1 = kalloc(512);
    // strcpy(data1, "a");
    // create_file("first_file", data1);

    // printf("%s\r\n", read_file("first_file")); 
    // print_fs();

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);

    currentPassword = (char*)malloc(6);
    strcpy(currentPassword, "12345\0");

    while (1) {
        printf("\r\n[CORDELL OS]: ");

        char* command = keyboard_read(1);
        execute_command(command);
            
        free(command);
    }

end:
    for (;;);
}

void execute_command(char* command) {
    if (strstr(command, "aiuto") == 0) {
        printf("\r\n> Usa l'aiuto per ottenere aiuto");
        printf("\r\n> Utilizzare clear per la pulizia dello schermo");
        printf("\r\n> Usa l'eco per l'eco");
        printf("\r\n> Utilizza la calc per i calcoli (+, -, * e /)");
        printf("\r\n> Utilizzare setpas per impostare la password per cordell");
        printf("\r\n> Utilizzare cordell per utilizzare i comandi cordell");
    }
    else if (strstr(command, "clear") == 0) 
        VGA_clrscr();
    else if (strstr(command, "eco") == 0) {
        char* echo = command + strlen("eco") + 1; 
        printf("\r\n%s", echo);
    }
    else if (strstr(command, "mkdir") == 0) {
        char* mkdir = command + strlen("mkdir") + 1; 
        create_temp_directory(mkdir);
    }
    else if (strstr(command, "rmdir") == 0) {
        char* rmdir = command + strlen("rmdir") + 1; 
        delete_temp_directory(rmdir);
    }
    else if (strstr(command, "dirs") == 0) {
        char* dirs = print_temp_dirs(); 
        printf("'\r\n");
        
        for (int i = 0; i < MAX_DIRECTORIES; i++) {
            if (dirs[i] != NULL) 
                printf("\t %s", i + 1, dirs[i]);
            else 
                break; // Exit the loop if you reach a NULL pointer
        }

    }
    else if (strstr(command, "calc") == 0) {
        char* expression = command + strlen("calc") + 1; 

        char* tokens[100];
        int tokenCount = 0;

        char* splitted = strtok(expression, " ");
        while(splitted) {
            tokens[tokenCount++] = splitted;
            splitted = strtok(NULL, " ");
        }

        printf("\r\n> Risposta: %s", calculator(tokens, tokenCount));
    }
    else if (strstr(command, "cordell") == 0) {
        char* cordellCommand = command + strlen("cordell") + 1; 

        printf("\r\n[PAROLA D'ORDINE]: ");
        char* password = keyboard_read(0);
        while (strcmp(password, currentPassword) != 0) {
            printf("\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
            free(password);

            password = keyboard_read(0);
        }

        free(password);

        if (strstr(cordellCommand, "setpas") == 0) {
            char* newPassword = cordellCommand + strlen("setpas") + 1; 
            free(currentPassword);

            char* buffer = (char*)malloc(strlen(newPassword) + 1);
            memset(buffer, 0, sizeof(buffer));
            if (buffer == NULL)
                return;

            strcpy(buffer, newPassword);

            currentPassword                             = buffer;
            currentPassword[strlen(currentPassword)]    = '\0';
        }
    } else 
        printf("\r\nComando sconosciuto. Forse hai dimenticato CORDELL?");
            
    printf("\r\n");
}

void print_fs() {
    char **files = list_files();

    for ( int currIdx = 0; currIdx < get_files_number(); currIdx++ ) 
        printf("File: %s\r\n", files[currIdx]);
    
    printf("==\r\n");
}