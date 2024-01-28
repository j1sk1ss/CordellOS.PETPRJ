#include "include/shell.h"

//   ____  _   _ _____ _     _     
//  / ___|| | | | ____| |   | |    
//  \___ \| |_| |  _| | |   | |    
//   ___) |  _  | |___| |___| |___ 
//  |____/|_| |_|_____|_____|_____|
                               
void shell() {

    char* curpath = (char*)malloc(4);
    strcpy(curpath, "BOOT");
    
    shell_start_screen();
    init_users();
    
    //=========================
    //  SHELL CORDELL PASSWORD
    //=========================

        if (cexists("boot\\shell.txt") == 0) {
            mkfile("boot", "shell.txt");
            fwrite("boot\\shell.txt", "12345");
        }

    //=========================
    //  SHELL CORDELL PASSWORD
    //=========================
    //  USER LOGIN
    //=========================

        int attempts = 0;
        struct User* user = NULL;
        while (user == NULL) {
            cprintf(FOREGROUND_LIGHT_RED, "\r\n[LOGIN]: ");
            char* user_login = input_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);

            cprintf(FOREGROUND_LIGHT_RED, "\r\n[PAROLA D'ORDINE]: ");
            char* user_pass = input_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);

            user = login(user_login, user_pass, 0);

            free(user_pass);
            free(user_login);

            if (user == NULL)
                if (++attempts > MAX_ATTEMPT_COUNT) {
                    cprintf(FOREGROUND_LIGHT_RED, "\r\nPassword o login errata, accedere alla modalita` ospite.\n\r");
                    user = (struct User*)malloc(sizeof(struct User*));
                    user->name = malloc(6);
                    strcpy(user->name, "guest\0");
                    
                    user->read_access   = 6;
                    user->write_access  = 6;
                    user->edit_access   = 6;

                    break;
                }
            else cprintf(FOREGROUND_LIGHT_RED, "\r\nPassword o login errata, accedere alla modalita` ospite.\n\r");
        }

        printf("\n");

    //=========================
    //  USER LOGIN
    //=========================

    while (1) {
        cprintf(FOREGROUND_GREEN, "\r\n[%s: CORDELL OS]", user->name);
        printf(" $%s> ", curpath);

        char* command = input_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        if (strstr(command, "cordell") == 0) execute_command(command + strlen("cordell") + 1, CORDELL_DERICTIVE, curpath, user);
        else execute_command(command, DEFAULT_DERICTIVE, curpath, user);

        free(command);
    }

    free(curpath);
}

void shell_start_screen() {
    printf("\n\n");

    cprintf(FOREGROUND_LIGHT_GREEN, "     .aMMMb  .aMMMb  dMMMMb  dMMMMb  dMMMMMP dMP     dMP        .aMMMb  .dMMMb \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "    dMP'VMP dMP'dMP dMP.dMP dMP VMP dMP     dMP     dMP        dMP'dMP dMP' VP \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "   dMP     dMP dMP dMMMMK' dMP dMP dMMMP   dMP     dMP        dMP dMP  VMMMb   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  dMP.aMP dMP.aMP dMP'AMF dMP.aMP dMP     dMP     dMP        dMP.aMP dP .dMP   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  VMMMP'  VMMMP' dMP dMP dMMMMP' dMMMMMP dMMMMMP dMMMMMP     VMMMP'  VMMMP'    \r\n");

    cprintf(FOREGROUND_LIGHT_AQUA, "\r\n\t Questo sistema operativo 'e in costruzione. [ver. 1.0.2a | 28.01.2024] \r\n");
}

//=========================
//  SHELL COMMANDS
//=========================

    void execute_command(char* command, int cordell_derictive, char* path, struct User* user) {

        //=========================
        //  PASSWORD VERIFICATION
        //=========================

            if (cordell_derictive == CORDELL_DERICTIVE) {
                cprintf(FOREGROUND_GREEN, "\r\n[PAROLA D'ORDINE]: ");
                char* password = input_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);
                int tries = 0;

                char* pass = fread("boot\\shell.txt");
                while (strcmp(password, pass) != 0) {
                    cprintf(FOREGROUND_RED, "\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");

                    free(password);
                    password = input_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);

                    if (++tries >= MAX_ATTEMPT_COUNT) return;
                }

                free(pass);
                free(password);
            }

            if (cordell_derictive == SUPER_DERICTIVE) cordell_derictive = CORDELL_DERICTIVE;

        //=========================
        //  PASSWORD VERIFICATION
        //=========================
        //  SPLIT COMMAND LINE TO ARGS
        //=========================

            char* command_line[100];
            int tokenCount = 0;

            char* splitted = strtok(command, " ");
            while(splitted != NULL) {
                command_line[tokenCount++]  = splitted;
                splitted                    = strtok(NULL, " ");
            }

        //=========================
        //  SPLIT COMMAND LINE TO ARGS
        //=========================
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //=========================

            if (strstr(command_line[0], COMMAND_HELP) == 0) {
                printf("\r\n> Usa [%s] per ottenere aiuto",                                     COMMAND_HELP);
                printf("\r\n> Utilizzare [%s] per la pulizia dello schermo",                    COMMAND_CLEAR);
                printf("\r\n> Usa [%s] per l'eco",                                              COMMAND_ECHO);
                printf("\r\n> Utilizza la [%s] per i calcoli (+, -, * e /)",                    COMMAND_CALCULATOR);
                printf("\r\n> Utilizzare cordell per utilizzare i comandi cordell");


                printf("\r\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                printf("\r\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);


                printf("\r\n> Usa [%s] <nome> per cretore dir",                                 COMMAND_CREATE_DIR);
                printf("\r\n> Usa [%s] <nome.extension> per cretore file",                      COMMAND_CREATE_FILE);
                printf("\r\n> Usa [%s] <nome> per elimita file",                                COMMAND_DELETE_CONTENT);

                printf("\r\n> Usa [%s] in dir per ottenere tutte le informazioni",              COMMAND_GO_TO_MANAGER);
                printf("\r\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                printf("\r\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                printf("\r\n> Usa [%s] per guardare tutto cosa in dir",                         COMMAND_LIST_DIR);


                printf("\r\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                printf("\r\n> Usa [%s] per modifica data in file",                              COMMAND_FILE_EDIT);
                printf("\r\n> Usa [%s] per run file",                                           COMMAND_FILE_RUN);
            }

            else if (strstr(command_line[0], COMMAND_VERSION) == 0) shell_start_screen();
            else if (strstr(command_line[0], COMMAND_CLEAR) == 0) clrscr();
            else if (strstr(command_line[0], COMMAND_ECHO) == 0) printf("\r\n%s", command_line[1]);

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                short data[7];
                get_datetime(&data);
                printf("\r\nGiorno: %i/%i/%i\tTempo: %i:%i:%i", data[3], data[4], data[5], 
                                                                data[2], data[1], data[0]);
            }

        //=========================
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //=========================
        //  FILE SYSTEM COMMANDS
        //=========================

            else if (strstr(command_line[0], COMMAND_CREATE_DIR) == 0) mkdir(path, command_line[1]);
                                                       
            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0) {
                str_uppercase(command_line[1]);
                char* dpath = FATLIB_change_path(path, command_line[1]);
                if (cexists(dpath) == 0) {
                    free(dpath);
                    printf("\nLa directory non esiste.");
                } else {
                    free(path);
                    path = dpath;
                }
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                char* upath = FATLIB_change_path(path, NULL);
                if (strlen(upath) <= 1) free(upath);
                else {
                    free(path);
                    path = upath;
                }
            }
            
            else if (strstr(command_line[0], COMMAND_DELETE_CONTENT) == 0) {
                char* rmpath = FATLIB_change_path(path, command_line[1]);
                if (cexists(rmpath) == 0) {
                    free(rmpath);
                    printf("Il contenuto non esiste.\n");
                    return;
                }

                free(rmpath);
                rmcontent(path, command_line[1]);
            }
            
            else if (strstr(command_line[0], COMMAND_CREATE_FILE) == 0) mkfile(path, command_line[1]);

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct UFATDirectory* directory   = opendir(path);
                struct UFATDirectory* current_dir = directory->subDirectory;
                struct UFATFile* current_file     = directory->files;

                printf("\n");
                while (current_dir != NULL) {
                    printf("\t%s", current_dir->name);
                    current_dir = current_dir->next;
                }
            
                printf("\n");
                while (current_file != NULL) {
                    printf("\t%s.%s", current_file->name, current_file->extension);
                    current_file = current_file->next;
                }

                FATLIB_unload_directories_system(directory);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                char* rpath = FATLIB_change_path(path, command_line[1]);
                char* data = fread(rpath);
                
                printf("\r\n%s", data);

                free(data);
                free(rpath);
            }

        //=========================
        //  FILE SYSTEM COMMANDS
        //=========================
        //  APPLICATIONS
        //=========================

            else if (strstr(command_line[0], COMMAND_GO_TO_MANAGER) == 0) open_file_manager(user, path);  

            else if (strstr(command_line[0], COMMAND_FILE_EDIT) == 0) {
                char* edit_path = FATLIB_change_path(path, command_line[1]); 
                text_editor_init(edit_path, BACKGROUND_BLACK + FOREGROUND_WHITE);
                
                free(edit_path);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                char* run_path    = FATLIB_change_path(path, command_line[1]);
                char* sector_data = fread(run_path);

                char* lines[100];
                int tokenCount = 0;

                char* splitted = strtok(sector_data, "\n");
                while(splitted != NULL) {
                    lines[tokenCount++] = splitted;
                    splitted = strtok(NULL, "\n");
                }

                for (int i = 0; i < tokenCount; i++)
                    if (cordell_derictive == CORDELL_DERICTIVE) execute_command(lines[i], SUPER_DERICTIVE, path, user);
                    else execute_command(lines[i], DEFAULT_DERICTIVE, path, user);

                free(sector_data);
            }

        //=========================
        //  APPLICATIONS  
        //=========================

            else printf("\r\nComando sconosciuto. Forse hai dimenticato CORDELL?");

        printf("\r\n");
    }

//=========================
//  SHELL COMMANDS
//=========================
