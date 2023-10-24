#include "include/shell.h"


char *currentPassword = "12345";

void shell() {
    shell_start_screen();
    init_directory(); 

    while (1) {
        char* path = get_full_temp_name();
        cprintf(FOREGROUND_GREEN, "\r\n[CORDELL OS]");
        printf(" $%s> ", path);

        char *command = keyboard_read(VISIBLE_KEYBOARD);
        if (strstr(command, "cordell") == 0)
            execute_command(command + strlen("cordell") + 1, CORDELL_ACCESS);
        else
            execute_command(command, DEFAULT_ACCESS);

        free(path);
        free(command);
    }
}

void shell_start_screen() {
    cprintf(FOREGROUND_LIGHT_GREEN, "  _____  ____  ____   ___    ___ ||    ||        ____    ____\r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "_|      ||  || || ||  || || ||   ||    ||       ||  ||  |    \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "||      ||  || ||_||  || || ||   ||    ||       ||  || ||    \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "||      ||  || |||    || || ||__ ||    ||       ||  || ||    \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "||      ||  || || ||  || || ||   ||    ||       ||  || ||    \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, " |_____  |__|  ||  || ||_|| ||__ ||___ ||___     |__|   |____\r\n");

    cprintf(FOREGROUND_AQUA, "\r\n Questo sistema operativo 'e in costruzione. \r\n");
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
                cprintf(FOREGROUND_GREEN, "\r\n[PAROLA D'ORDINE]: ");
                char* password = keyboard_read(HIDDEN_KEYBOARD);
                int tries = 0;

                while (strcmp(password, currentPassword) != 0) {
                    cprintf(FOREGROUND_RED, "\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    free(password);

                    password = keyboard_read(HIDDEN_KEYBOARD);
                    if (++tries >= MAX_ATTEMPT_COUNT) 
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

            if (strstr(command_line[0], COMMAND_HELP) == 0) {
                printf("\r\n> Usa [%s] per ottenere aiuto",                             COMMAND_HELP);
                printf("\r\n> Utilizzare [%s] per la pulizia dello schermo",            COMMAND_CLEAR);
                printf("\r\n> Usa [%s] per l'eco",                                      COMMAND_ECHO);
                printf("\r\n> Utilizza la [%s] per i calcoli (+, -, * e /)",            COMMAND_CALCULATOR);
                printf("\r\n> Utilizzare [%s] per impostare la password per cordell",   COMMAND_PASS);
                printf("\r\n> Utilizzare cordell per utilizzare i comandi cordell");

                printf("\r\n> Utilizzare [%s] che guardare tutte drivi",                COMMAND_DRIVES_LIST);
                printf("\r\n> Utilizzare [%s] con numerico per guardare sector data",   COMMAND_GET_HDD_SECTOR);

                printf("\r\n> Usa [%s] <nome> per cretore dir",                         COMMAND_CREATE_DIR);
                printf("\r\n> Usa [%s] <accesso> <nome> per cretore file",              COMMAND_CREATE_FILE);
                printf("\r\n> Usa [%s] <nome> per elimita dir",                         COMMAND_DELETE_FILE);
                printf("\r\n> Usa [%s] <nome> per entranto dir",                        COMMAND_IN_DIR);
                printf("\r\n> Usa [%s] per uscire di dir",                              COMMAND_OUT_DIR);
                printf("\r\n> Usa [%s] per guardare tutto cosa in dir",                 COMMAND_LIST_DIR);

                printf("\r\n> Usa [%s] per guardare tutto data in file",                COMMAND_FILE_VIEW);
                printf("\r\n> Usa [%s] per modifica data in file",                      COMMAND_FILE_EDIT);
                printf("\r\n> Usa [%s] per run file",                                   COMMAND_FILE_RUN);
            }

            else if (strstr(command_line[0], COMMAND_CLEAR) == 0) 
                VGA_clrscr();

            else if (strstr(command_line[0], COMMAND_ECHO) == 0) 
                printf("\r\n%s", command_line[1]);

            else if (strstr(command_line[0], COMMAND_PASS) == 0) {
                if (access_level == 0) {
                    printf("\r\n%s\r\n", CORDELL_ATTENTION);                 
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

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                datetime_read_rtc();
                printf("\r\n%i/%i/%i\r\n%i:%i:%i", datetime_day, datetime_month, datetime_year, datetime_hour, datetime_minute, datetime_second);
            }

        //
        //
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  FILE SYSTEM COMMANDS
        //
        //
        
            else if (strstr(command_line[0], COMMAND_GET_HDD_SECTOR) == 0) {
                int LBA = command_line[1];

                uint8_t slavebit = 0;
                uint8_t sectorCount = 1;

                outportb(0x1F6, 0xE0 | (slavebit << 4) | ((LBA >> 24) & 0x0F));
                outportb(0x1F1, 0x00);
                outportb(0x1F2, sectorCount);
                outportb(0x1F3, (uint8_t)(LBA & 0xFF));
                outportb(0x1F4, (uint8_t)((LBA >> 8) & 0xFF));
                outportb(0x1F5, (uint8_t)((LBA >> 16) & 0xFF));
                outportb(0x1F7, 0x20);

                while ((inportb(0x1F7) & 0x8) == 0) 
                    continue;

                for (int n = 0; n < 256; n++) {
                    uint16_t value = inportw(0x1F0);
                    printf("%c%c", value & 0xFF, (value >> 8));
                } 
            }

            else if (strstr(command_line[0], COMMAND_DRIVES_LIST) == 0) {
                printf("\r\n");
                cmos_select_register(0x10);
                
                uint8_t driveDesc   = cmos_read();
                uint8_t drive0Desc  = driveDesc >> 4;
                uint8_t drive1Desc  = driveDesc & 0x0F;

                printf("DRIVE 0:  %s\r\n", drive_get_description(drive0Desc));
                printf("DRIVE 1:  %s\r\n", drive_get_description(drive1Desc));
                printf("DRIVE 2:  %i\r\n", harddrive_detect_devtype(0, 0x1F0, 0x3F6));
                printf("DRIVE 3:  %i\r\n", harddrive_detect_devtype(1, 0x1F0, 0x3F6));
            }

        //
        //
        //
        //  FILE SYSTEM COMMANDS
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  TEMP FILE SYSTEM COMMANDS (TEMP FILE SYSTEM CREATED FOR SIMULATION OF WORKING FS (WIP))
        //
        //

            else if (strstr(command_line[0], COMMAND_CREATE_DIR) == 0)                              // Create new dir
                create_temp_directory(command_line[1]);                                             // Name placed as second arg
            
            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0)                                  // Move to dir           
                move_to_temp_directory(command_line[1]);                                            //
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0)                                 // Up from dir
                up_from_temp_directory();                                                           //
            
            else if (strstr(command_line[0], COMMAND_DELETE_DIR) == 0)                              // Delete dir
                switch (access_level) {
                    case DEFAULT_ACCESS:
                        delete_temp_directory(command_line[1]); 
                    break;

                    case CORDELL_ACCESS:
                        cordell_delete_temp_directory(command_line[1]);
                    break;
                }                                              
            
            else if (strstr(command_line[0], COMMAND_CREATE_FILE) == 0) {                           //
                char* text =            
                    command             
                     + strlen(command_line[0])          
                     + strlen(command_line[1])          
                     + strlen(command_line[2])          
                     + 3;           
                
                create_temp_file(command_line[1], command_line[2], text);                           // Name placed as third arg
            }           
                        
            else if (strstr(command_line[0], COMMAND_DELETE_FILE) == 0)                             // Delete file by name
                switch (access_level) {
                    case DEFAULT_ACCESS:
                        delete_temp_file(command_line[1]); 
                    break;

                    case CORDELL_ACCESS:
                        cordell_delete_temp_file(command_line[1]);
                    break;
                }   
            
            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {                              // List of all files
                struct TempDirectory* current_dir = get_current_directory()->subDirectory;          // Print dirs
                if (current_dir != NULL) {                                                          //
                    printf("\r\n\t%s", current_dir->name);                                          //
            
                    while (current_dir->next != NULL) {                                             //
                        current_dir = current_dir->next;                                            //
                        printf("\t%s", current_dir->name);                                          //
                    }                                                                               //
                }                                                                                   //
            
                struct TempFile* current = get_current_directory()->files;                          // Print files
                if (current != NULL) {                                                              //
                    printf("\r\n\t%s", current->name);                                              //
            
                    while (current->next != NULL) {                                                 //
                        current = current->next;                                                    //
                        printf("\t%s", current->name);                                              //
                    }                                                                               //
                }                                                                                   //
            }                                                                                       //

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                struct TempFile* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\n%s\r\n", CORDELL_ATTENTION);
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

            else if (strstr(command_line[0], COMMAND_CALCULATOR) == 0) {
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

            else if (strstr(command_line[0], COMMAND_FILE_EDIT) == 0) {
                struct TempFile* file = find_temp_file(command_line[1]);
                if (file == NULL)
                    return;
                
                if (strcmp(file->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\n%s\r\n", CORDELL_ATTENTION);
                    return;
                }

                VGA_clrscr();
                printf("Stai modificando il file. Utilizzare CAPSLOCK per uscire.\r\n\r\n");

                file->content = keyboard_edit(file->content);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                struct TempFile* execute = find_temp_file(command_line[1]);
                if (execute == NULL)
                    return;
                
                if (strcmp(execute->fileType, "0") == 0 && access_level == DEFAULT_ACCESS) {
                    printf("\r\n%s\r\n", CORDELL_ATTENTION);
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