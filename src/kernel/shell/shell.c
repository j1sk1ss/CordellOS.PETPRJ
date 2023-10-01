#include "shell.h"


char* currentPassword;

void shell() {
    shell_start_screen();
    init_directory(); 

    currentPassword = (char*)malloc(6);
    strcpy(currentPassword, "12345\0");

    while (1) {
        char* path = get_full_temp_name();
        printf("\r\n[CORDELL OS] $%s> ", path);

        char* command = keyboard_read(VISIBLE_KEYBOARD);
        if (strstr(command, "cordell") == 0)
            execute_command(command + strlen("cordell"), CORDELL_ACCESS);
        else
            execute_command(command, DEFAULT_ACCESS);

        free(path);
        free(command);
    }
}

void shell_start_screen() {
    
    printf("  _____  ____  ____   ___    ___ ||    ||        ____    ____\r\n");
    printf("_|      ||  || || ||  || || ||   ||    ||       ||  ||  |    \r\n");
    printf("||      ||  || ||_||  || || ||   ||    ||       ||  || ||    \r\n");
    printf("||      ||  || |||    || || ||__ ||    ||       ||  || ||    \r\n");
    printf("||      ||  || || ||  || || ||   ||    ||       ||  || ||    \r\n");
    printf(" |_____  |__|  ||  || ||_|| ||__ ||___ ||___     |__|   |____\r\n");
    printf("\r\n Questo sistema operativo 'e in costruzione. \r\n");
}

///////////////////////////////////////
//
//  SHELL COMMANDS
//
//

    void execute_command(char* command, int access_level) {

        ////////////////////////////////
        //
        //  PASSWORD VERIFICATION
        //

            if (access_level == 1) {
                printf("\r\n[PAROLA D'ORDINE]: ");
                char* password = keyboard_read(HIDDEN_KEYBOARD);

                int tries = 0;

                while (strcmp(password, currentPassword) != 0) {
                    printf("\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    free(password);

                    password = keyboard_read(HIDDEN_KEYBOARD);
                    if (tries >= 4) 
                        return;
                }

                free(password);            
            }

        //
        //  PASSWORD VERIFICATION
        //
        ////////////////////////////////
        //
        //  SPLIT COMMAND LINE TO ARGS
        //

            char* command_line[100];
            int tokenCount = 0;

            char* command_for_split = (char*)malloc(strlen(command));
            strcpy(command_for_split, command);

            char* splitted = strtok(command_for_split, " ");
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

                printf("\r\n> Usa view per guardare tutto data in file");
                printf("\r\n> Usa nano per modifica data in file");
                printf("\r\n> Usa run per run file");
            }

            else if (strstr(command_line[0], "clear") == 0) 
                VGA_clrscr();

            else if (strstr(command_line[0], "eco") == 0) {
                char* echo = command_line[1]; 
                printf("\r\n%s", echo);
            }

            if (strstr(command_line[0], "setpas") == 0) {
                if (access_level == 0) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");                 
                    return;
                }

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

        //
        //
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  TEMP FILE SYSTEM COMMANDS (TEMP FILE SYSTEM CREATED FOR SIMULATION OF WORKING FS (WIP))
        //
        //


            else if (strstr(command_line[0], "mkdir") == 0)                                         // Create new dir
                create_temp_directory(command_line[1]);                                             // Name placed as second arg
            
            else if (strstr(command_line[0], "cd") == 0)                                            // Move to dir           
                move_to_temp_directory(command_line[1]);                                            //
            
            else if (strstr(command_line[0], "..") == 0)                                            // Up from dir
                up_from_temp_directory();                                                           //
            
            else if (strstr(command_line[0], "rmdir") == 0)                                         // Delete dir
                switch (access_level) {
                    case DEFAULT_ACCESS:
                        delete_temp_directory(command_line[1]); 
                    break;

                    case CORDELL_ACCESS:
                        cordell_delete_temp_directory(command_line[1]);
                    break;
                }                                              
            
            else if (strstr(command_line[0], "mkfile") == 0) {                                      //
                char* text =            
                    command             
                     + strlen(command_line[0])          
                     + strlen(command_line[1])          
                     + strlen(command_line[2])          
                     + 3;           
                
                create_temp_file(command_line[1], command_line[2], text);                           // Name placed as third arg
            }           
                        
            else if (strstr(command_line[0], "rmfile") == 0)                                        // Delete file by name
                switch (access_level) {
                    case DEFAULT_ACCESS:
                        delete_temp_file(command_line[1]); 
                    break;

                    case CORDELL_ACCESS:
                        cordell_delete_temp_file(command_line[1]);
                    break;
                }   
            
            else if (strstr(command_line[0], "dir") == 0) {                                         // List of all files
                struct Directory* current_dir = get_current_directory()->subDirectory;              // Print dirs
                if (current_dir != NULL) {                                                          //
                    printf("\t%s", current_dir->name);                                              //
            
                    while (current_dir->next != NULL) {                                             //
                        current_dir = current_dir->next;                                            //
                        printf("\t%s", current_dir->name);                                          //
                    }                                                                               //
                }                                                                                   //
            
                struct File* current = get_current_directory()->files;                              // Print files
                if (current != NULL) {                                                              //
                    printf("\t%s", current->name);                                                  //
            
                    while (current->next != NULL) {                                                 //
                        current = current->next;                                                    //
                        printf("\t%s", current->name);                                              //
                    }                                                                               //
                }                                                                                   //
            }                                                                                       //

            else if (strstr(command_line[0], "view") == 0) {
                struct File* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");
                    return;
                }

                printf("\r\n%s", file->content);
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

                char* expression =            
                    command             
                     + strlen(command_line[0])          
                     + 1;  

                char* splitted = strtok(expression, " ");
                while(splitted) {
                    tokens[tokenCount++] = splitted;
                    splitted = strtok(NULL, " ");
                }

                printf("\r\n> Risposta: %s", calculator(tokens, tokenCount));
            }

            else if (strstr(command_line[0], "nano") == 0) {
                struct File* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");
                    return;
                }

                VGA_clrscr();
                printf("You are editing file. Use CAPSLOCK to exit.\r\n\r\n");

                file->content = keyboard_edit(file->content);
            }

            else if (strstr(command_line[0], "run") == 0) {
                struct File* execute = find_temp_file(command_line[1]);
                if (execute == NULL)
                    return;
                
                if (strcmp(execute->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\nYou don`t have permissions. Use cordell.\r\n");
                    return;
                }

                char* command_for_split = (char*)malloc(strlen(execute->content));  // not work
                strcpy(command_for_split, execute->content);

                char* lines[100];
                int tokenCount = 0;

                char* splitted = strtok(command_for_split, "\n");
                while(splitted) {
                    lines[tokenCount++] = splitted;
                    splitted = strtok(NULL, "\n");
                }

                for (int i = 0; i < tokenCount; i++)
                    execute_command(lines[i], 0);

                free(command_for_split);
            }

        //
        //
        //  APPLICATIONS  
        //
        ///////////////////////////////////////////////////////////////////////////////////////////

            else 
                printf("\r\nComando sconosciuto. Forse hai dimenticato CORDELL?");


        free(command_line);
        free(command_for_split);

        printf("\r\n");
    }

//
//
//  SHELL COMMANDS
//
////////////////////////////////////////////