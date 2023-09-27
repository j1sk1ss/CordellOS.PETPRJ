#include <stdint.h>

#include <boot/bootparams.h>

#include "../libs/core/shared/include/allocator.h"
#include "../libs/core/shared/include/ata.h"
#include "../libs/core/shared/include/file_system.h"
#include "../libs/core/shared/include/temp_file_system.c"

#include <include/memory.h>
#include <include/debug.h>
#include <include/string.h>
#include <include/stdio.h>
#include <include/hal.h>
#include <include/irq.h>
#include <include/io.h>
#include <include/calculator.h>
#include <include/cordell_nano.h>

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

    // One of variants of implementation FS (look ..file_system/file_system.h")

    // char *data1 = kalloc(512);
    // strcpy(data1, "a");
    // create_file("first_file", data1);

    // printf("%s\r\n", read_file("first_file")); 
    // print_fs();

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);

    currentPassword = (char*)malloc(6);
    strcpy(currentPassword, "12345\0");

    while (1) {
        char* path = get_full_temp_name();
        printf("\r\n[CORDELL OS] $%s: ", path);

        char* command = keyboard_read(VISIBLE_KEYBOARD);
        execute_command(command);
            
        free(path);
        free(command);
    }

end:
    for (;;);
}





///////////////////////////////////////
//
//  SHELL COMMANDS
//
//

    void execute_command(char* command) {

        ////////////////////////////////
        //
        //  SPLIT COMMAND LINE TO ARGS
        //

            char* command_line[100];
            int tokenCount = 0;

            char* splitted = strtok(command, " ");
            while(splitted) {
                command_line[tokenCount++]  = splitted;
                splitted                    = strtok(NULL, " ");
            }

        //
        //  SPLIT COMMAND LINE TO ARGS
        //
        ////////////////////////////////
        //
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //
        //

            if (strstr(command_line[0], "aiuto") == 0) {
                printf("\r\n> Usa l'aiuto per ottenere aiuto");
                printf("\r\n> Utilizzare clear per la pulizia dello schermo");
                printf("\r\n> Usa l'eco per l'eco");
                printf("\r\n> Utilizza la calc per i calcoli (+, -, * e /)");
                printf("\r\n> Utilizzare setpas per impostare la password per cordell");
                printf("\r\n> Utilizzare cordell per utilizzare i comandi cordell");

                printf("\r\n> Usa mkdir <nome> per cretore dir");
                printf("\r\n> Usa mkfile <accesso> <nome> per cretore file");
                printf("\r\n> Usa rmdir <nome> per elimita dir");
                printf("\r\n> Usa cd <nome> per entranto dir");
                printf("\r\n> Usa .. per uscire di dir");
                printf("\r\n> Usa dir per guardare tutto cosa in dir");
            }

            else if (strstr(command_line[0], "clear") == 0) 
                VGA_clrscr();

            else if (strstr(command_line[0], "eco") == 0) {
                char* echo = command_line[1]; 
                printf("\r\n%s", echo);
            }

        //
        //
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  TEMP FILE SYSTEM COMMANDS (TEMP FILE SYSTEM CREATED FOR SIMULATION OF WORKING FS (WIP))
        //
        //


            else if (strstr(command_line[0], "mkdir") == 0)                             // Create new dir
                create_temp_directory(command_line[1]);                                 // Name placed as second arg

            else if (strstr(command_line[0], "cd") == 0)                                // Move to dir           
                move_to_temp_directory(command_line[1]);                                //

            else if (strstr(command_line[0], "..") == 0)                                // Up from dir
                up_from_temp_directory();                                               //

            else if (strstr(command_line[0], "rmdir") == 0)                             // Delete dir
                delete_temp_directory(command_line[1]);                                 //

            else if (strstr(command_line[0], "mkfile") == 0) {                          //
                char* text = 
                    command + strlen(command_line[1]) + strlen(command_line[2]) + 2;
                
                create_temp_file(command_line[1], command_line[2], text);               // Name placed as third arg
            }
            
            else if (strstr(command_line[0], "rmfile") == 0)                            // Delete file by name
                delete_temp_file(command_line[1]);                                      //

            else if (strstr(command_line[0], "dir") == 0) {                             // List of all files
                printf("\r\nDirs: \t");                                                 //
                
                struct Directory* current_dir = currentDirectory->subDirectory;         // Print dirs
                if (current_dir != NULL) {                                              //
                    printf("\t%s", current_dir->name);                                  //

                    while (current_dir->next != NULL) {                                 //
                        current_dir = current_dir->next;                                //
                        printf("\t%s", current_dir->name);                              //
                    }                                                                   //
                }                                                                       //
                else                                                                    //
                    printf("\r\nNo dirs");                                              //

                printf("\r\nFiles: \t");                                                //

                struct File* current = currentDirectory->files;                         // Print files
                if (current != NULL) {                                                  //
                    printf("\t%s", current->name);                                      //

                    while (current->next != NULL) {                                     //
                        current = current->next;                                        //
                        printf("\t%s", current->name);                                  //
                    }                                                                   //
                }                                                                       //
                else                                                                    //
                    printf("\r\nNo files");                                             //
            }                                                                           //

            else if (strstr(command_line[0], "view") == 0) {
                struct File* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");
                    return;
                }

                printf("\r\nContent of %s: %s", file->name, file->content);
            }

        //
        //
        //  TEMP FILE SYSTEM COMMANDS (TEMP FILE SYSTEM CREATED FOR SIMULATION OF WORKING FS (WIP))
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  APPLICATIONS
        //
        //

            else if (strstr(command_line[0], "calc") == 0) {
                char* tokens[100];
                int tokenCount = 0;

                char* splitted = strtok(command_line[1], ".");
                while(splitted) {
                    tokens[tokenCount++] = splitted;
                    splitted = strtok(NULL, ".");
                }

                printf("\r\n> Risposta: %s", calculator(tokens, tokenCount));
            }

            else if (strstr(command_line[0], "cordell-nano") == 0) {
                struct File* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");
                    return;
                }

                file->content = cordell_nano_editor(file->content);
            }

        //
        //
        //  APPLICATIONS  
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  CORDELL COMMANDS (COMMANDS THAT NEED CORDELL SPEC. WORD)
        //
        //


            else if (strstr(command_line[0], "cordell") == 0) {
                printf("\r\n[PAROLA D'ORDINE]: ");
                char* password = keyboard_read(HIDDEN_KEYBOARD);
                while (strcmp(password, currentPassword) != 0) {
                    printf("\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    free(password);

                    password = keyboard_read(0);
                }

                free(password);

                if (strstr(command_line[1], "setpas") == 0) {
                    char* newPassword = command_line[2]; 
                    free(currentPassword);

                    char* buffer = (char*)malloc(strlen(newPassword) + 1);
                    memset(buffer, 0, sizeof(buffer));
                    if (buffer == NULL)
                        return;

                    strcpy(buffer, newPassword);

                    currentPassword                             = buffer;
                    currentPassword[strlen(currentPassword)]    = '\0';
                }

                /////////////////////////////////////////////////////////
                //
                //  VFS CORDELL COMMANDS
                //

                    if (strstr(command_line[1], "rmdir") == 0) 
                        cordell_delete_temp_directory(command_line[2]); 
                    
                    else if (strstr(command_line[1], "view") == 0) {
                        struct File* file = find_temp_file(command_line[2]);
                        if (file == NULL)
                            return;

                        printf("\r\nContent of %s: %s", file->name, file->content);
                    }

                //
                //  VFS CORDELL COMMANDS
                //
                /////////////////////////////////////////////////////////

            } else 
                printf("\r\nComando sconosciuto. Forse hai dimenticato CORDELL?");


        //
        //
        //  CORDELL COMMANDS (COMMANDS THAT NEED CORDELL SPEC. WORD)
        //
        ///////////////////////////////////////////////////////////////////////////////////////////


        printf("\r\n");
    }

    void print_fs() {
        char **files = list_files();

        for ( int currIdx = 0; currIdx < get_files_number(); currIdx++ ) 
            printf("File: %s\r\n", files[currIdx]);
        
        printf("==\r\n");
    }

//
//
//  SHELL COMMANDS
//
////////////////////////////////////////////