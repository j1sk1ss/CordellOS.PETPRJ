#include "include/shell.h"

//   ____  _   _ _____ _     _     
//  / ___|| | | | ____| |   | |    
//  \___ \| |_| |  _| | |   | |    
//   ___) |  _  | |___| |___| |___ 
//  |____/|_| |_|_____|_____|_____|
                               
struct User* user = NULL;

void shell() {
    char* curpath = (char*)SYS_malloc(4);
    strcpy(curpath, "BOOT");
    
    shell_start_screen();
    init_users();
    
    /////////////
    //  SHELL CORDELL PASSWORD
    //
        if (SYS_cexists("boot\\shell.txt") == 0) {
            SYS_mkfile("boot", "shell.txt");
            SYS_fwrite("boot\\shell.txt", "12345");
        }
    //
    //  SHELL CORDELL PASSWORD
    /////////////

    ////////////////////////////////////////////////////
    //
    //  USER LOGIN
    //
        int attempts = 0;
        while (user == NULL) {
            cprintf(FOREGROUND_LIGHT_RED, "\r\n[LOGIN]: ");
            char* user_login = SYS_keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);

            cprintf(FOREGROUND_LIGHT_RED, "\r\n[PAROLA D'ORDINE]: ");
            char* user_pass = SYS_keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);

            user = login(user_login, user_pass, 0);

            SYS_free(user_login);
            SYS_free(user_pass);

            if (user == NULL)
                if (++attempts > MAX_ATTEMPT_COUNT) {
                    cprintf(FOREGROUND_LIGHT_RED, "\r\nPassword o login errata, accedere alla modalita` ospite.\n\r");
                    user = (struct User*)SYS_malloc(sizeof(struct User*));
                    user->name = SYS_malloc(6);
                    strcpy(user->name, "guest");
                    
                    user->read_access   = 6;
                    user->write_access  = 6;
                    user->edit_access   = 6;

                    break;
                }
            else cprintf(FOREGROUND_LIGHT_RED, "\r\nPassword o login errata, accedere alla modalita` ospite.\n\r");
        }

        printf("\n");
    //
    //  USER LOGIN
    //
    ////////////////////////////////////////////////////

    while (1) {
        cprintf(FOREGROUND_GREEN, "\r\n[%s: CORDELL OS]", user->name);
        printf(" $%s> ", curpath);

        char* command = SYS_keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        if (strstr(command, "cordell") == 0) execute_command(command + strlen("cordell") + 1, CORDELL_DERICTIVE, curpath);
        else execute_command(command, DEFAULT_DERICTIVE, curpath);

        SYS_free(command);
    }
}

void shell_start_screen() {
    printf("\n\n");

    cprintf(FOREGROUND_LIGHT_GREEN, "     .aMMMb  .aMMMb  dMMMMb  dMMMMb  dMMMMMP dMP     dMP        .aMMMb  .dMMMb \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "    dMP'VMP dMP'dMP dMP.dMP dMP VMP dMP     dMP     dMP        dMP'dMP dMP' VP \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "   dMP     dMP dMP dMMMMK' dMP dMP dMMMP   dMP     dMP        dMP dMP  VMMMb   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  dMP.aMP dMP.aMP dMP'AMF dMP.aMP dMP     dMP     dMP        dMP.aMP dP .dMP   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  VMMMP'  VMMMP' dMP dMP dMMMMP' dMMMMMP dMMMMMP dMMMMMP     VMMMP'  VMMMP'    \r\n");

    cprintf(FOREGROUND_LIGHT_AQUA, "\r\n\t Questo sistema operativo 'e in costruzione. [ver. 1.0.1b | 16.01.2024] \r\n");
}

///////////////////////////////////////
//
//  SHELL COMMANDS
//
    void execute_command(char* command, int cordell_derictive, char* path) {

        ////////////////////////////////
        //
        //  PASSWORD VERIFICATION
        //
            if (cordell_derictive == CORDELL_DERICTIVE) {
                cprintf(FOREGROUND_GREEN, "\r\n[PAROLA D'ORDINE]: ");
                char* password = SYS_keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);
                int tries = 0;

                char* pass = SYS_fread("boot\\shell.txt");
                while (strcmp(password, pass) != 0) {
                    cprintf(FOREGROUND_RED, "\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    SYS_free(password);

                    password = SYS_keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);
                    if (++tries >= MAX_ATTEMPT_COUNT) 
                        return;
                }

                SYS_free(password);
                SYS_free(pass);
            }

            if (cordell_derictive == SUPER_DERICTIVE)
                cordell_derictive = CORDELL_DERICTIVE;
        //
        //  PASSWORD VERIFICATION
        //
        ////////////////////////////////
        //
        //  SPLIT COMMAND LINE TO ARGS
        //
            char* command_line[100];
            int tokenCount = 0;

            char* command_for_split = (char*)SYS_malloc(strlen(command));
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
            if (strstr(command_line[0], COMMAND_HELP) == 0) {
                printf("\r\n> Usa [%s] per ottenere aiuto",                                     COMMAND_HELP);
                printf("\r\n> Utilizzare [%s] per la pulizia dello schermo",                    COMMAND_CLEAR);
                printf("\r\n> Usa [%s] per l'eco",                                              COMMAND_ECHO);
                printf("\r\n> Utilizza la [%s] per i calcoli (+, -, * e /)",                    COMMAND_CALCULATOR);
                printf("\r\n> Utilizzare cordell per utilizzare i comandi cordell");


                printf("\r\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                printf("\r\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);


                printf("\r\n> Usa [%s] <nome> per cretore dir",                                 COMMAND_CREATE_DIR);
                printf("\r\n> Usa [%s] <nome> <path> per elimita dir",                          COMMAND_DELETE_DIR);

                printf("\r\n> Usa [%s] <accesso [x x x]> <nome> <extension> per cretore file",  COMMAND_CREATE_FILE);
                printf("\r\n> Usa [%s] <nome> <path> per elimita file",                         COMMAND_DELETE_FILE);

                printf("\r\n> Usa [%s] in dir per ottenere tutte le informazioni",              COMMAND_GO_TO_MANAGER);
                printf("\r\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                printf("\r\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                printf("\r\n> Usa [%s] per guardare tutto cosa in dir",                         COMMAND_LIST_DIR);


                printf("\r\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                printf("\r\n> Usa [%s] per modifica data in file",                              COMMAND_FILE_EDIT);
                printf("\r\n> Usa [%s] per run file",                                           COMMAND_FILE_RUN);
            }

            else if (strstr(command_line[0], COMMAND_VERSION) == 0) shell_start_screen();
            else if (strstr(command_line[0], COMMAND_CLEAR) == 0) SYS_clrs();
            else if (strstr(command_line[0], COMMAND_ECHO) == 0) printf("\r\n%s", command_line[1]);

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                short data[7];
                SYS_get_datetime(&data);
                printf("\r\nGiorno: %i/%i/%i\tTempo: %i:%i:%i", data[3], data[4], data[5], 
                                                                data[2], data[1], data[0]);
            }
        //
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  FILE SYSTEM COMMANDS
        //
            else if (strstr(command_line[0], COMMAND_CREATE_DIR) == 0) SYS_mkdir(path, command_line[1]);
            else if (strstr(command_line[0], COMMAND_GO_TO_MANAGER) == 0) open_file_manager(user);                                             

            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0) {
                char* dname = SYS_malloc(strlen(command_line[1]));
                strcpy(dname, command_line[1]);
                str_uppercase(dname);

                path = (FATLIB_change_path(path, dname));
                if (SYS_cexists(path) == 0) {
                    path = (FATLIB_change_path(path, NULL));
                    printf("\nDirectory not exists.");
                }

                SYS_free(dname);
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                path = (FATLIB_change_path(path, NULL));
                if (strlen(path) <= 1)
                    path = "BOOT";
            }
            
            else if (strstr(command_line[0], COMMAND_DELETE_DIR) == 0) {
                path = (FATLIB_change_path(path, command_line[1]));
                if (SYS_cexists(path) == 0) {
                    path = (FATLIB_change_path(path, NULL));
                    printf("Content is not a directory exists.\n");
                    return;
                }

                path = (FATLIB_change_path(path, NULL));
                SYS_rmcontent(path, command_line[1]);
            }
            
            else if (strstr(command_line[0], COMMAND_CREATE_FILE) == 0) SYS_mkfile(path, command_line[1]);

            else if (strstr(command_line[0], COMMAND_DELETE_FILE) == 0)  {    
                path = (FATLIB_change_path(path, command_line[1]));
                if (SYS_cexists(path) == 0) {
                    path = (FATLIB_change_path(path, NULL));
                    printf("Content is not a directory exists.\n");
                    return;
                }

                path = (FATLIB_change_path(path, NULL));
                SYS_rmcontent(path, command_line[1]);
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct UFATDirectory* directory = SYS_opendir(path);
                struct UFATDirectory* current_dir = directory->subDirectory;
                struct UFATFile* current_file = directory->files;

                if (current_dir != NULL) {
                    printf("\r\n\t%s", current_dir->name);
            
                    while (current_dir->next != NULL) {
                        current_dir = current_dir->next;
                        printf("\t%s", current_dir->name);
                    }
                }

                if (current_file != NULL) {
                    printf("\r\n\t%s.%s", current_file->name, current_file->extension);
            
                    while (current_file->next != NULL) {
                        current_file = current_file->next;
                        printf("\t%s.%s", current_file->name, current_file->extension);
                    }
                }

                FATLIB_unload_files_system(current_file);
                FATLIB_unload_directories_system(current_dir);
                FATLIB_unload_directories_system(directory);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                path = (FATLIB_change_path(path, command_line[1]));
                char* data = SYS_fread(path);
                path = (FATLIB_change_path(path, NULL));

                printf("\r\n%s", data);
                SYS_free(data);
            }
        //
        //  FILE SYSTEM COMMANDS
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  APPLICATIONS
        //
            else if (strstr(command_line[0], COMMAND_FILE_EDIT) == 0) {
                path = FATLIB_change_path(path, command_line[1]); 
                text_editor_init(path, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                path = FATLIB_change_path(path, NULL);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                char* sector_data = SYS_fread(path);
                char* command_for_split = (char*)SYS_malloc(strlen(sector_data));
                strcpy(command_for_split, sector_data);

                char* lines[100];
                int tokenCount = 0;

                char* splitted = strtok(command_for_split, "\n");
                while(splitted) {
                    lines[tokenCount++] = splitted;
                    splitted = strtok(NULL, "\n");
                }

                for (int i = 0; i < tokenCount; i++)
                    if (cordell_derictive == CORDELL_DERICTIVE) execute_command(lines[i], SUPER_DERICTIVE, path);
                    else execute_command(lines[i], DEFAULT_DERICTIVE, path);

                SYS_free(command_for_split);
                SYS_free(sector_data);
            }
        //
        //  APPLICATIONS  
        //
        ///////////////////////////////////////////////////////////////////////////////////////////

            else 
                printf("\r\nComando sconosciuto. Forse hai dimenticato CORDELL?");


        SYS_free(command_line);
        SYS_free(command_for_split);

        printf("\r\n");
    }
//
//  SHELL COMMANDS
//
////////////////////////////////////////////
