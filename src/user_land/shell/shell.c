#include "include/shell.h"
// TODO: fat.c
//   ____  _   _ _____ _     _     
//  / ___|| | | | ____| |   | |    
//  \___ \| |_| |  _| | |   | |    
//   ___) |  _  | |___| |___| |___ 
//  |____/|_| |_|_____|_____|_____|
                                

struct File* currentPassword;
struct User* user = NULL;

void shell() {
    user = (struct User*)malloc(sizeof(struct User*));                 
    user->read_access   = 6;
    user->write_access  = 6;
    user->edit_access   = 6;

    open_file_manager(user);

    return;

    shell_start_screen();
    init_users();
    
    /////////////
    //  SHELL CORDELL PASSWORD
    //

        if (FS_file_exist("/shell") != 1) {
            FS_create_file(0, 0, 0, "/shell", "txt", ATA_find_empty_sector(SYS_FILES_SECTOR_OFFSET));
            currentPassword = FS_find_file("/shell", NULL);
            FS_write_file(currentPassword, "12345");
        } else currentPassword = FS_find_file("/shell", NULL);

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
            char* user_login = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);

            cprintf(FOREGROUND_LIGHT_RED, "\r\n[PAROLA D'ORDINE]: ");
            char* user_pass = keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);

            user = login(user_login, user_pass, 0);

            free(user_login);
            free(user_pass);

            if (user == NULL)
                if (++attempts > MAX_ATTEMPT_COUNT) {
                    cprintf(FOREGROUND_LIGHT_RED, "\r\nPassword o login errata, accedere alla modalita` ospite.\n\r");
                    user = (struct User*)malloc(sizeof(struct User*));
                    
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
        char* path = FS_get_full_temp_name();
        cprintf(FOREGROUND_GREEN, "\r\n[%s: CORDELL OS]", user->name);
        printf(" $%s> ", path);

        char* command = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        if (strstr(command, "cordell") == 0)
            execute_command(command + strlen("cordell") + 1, CORDELL_DERICTIVE);
        else
            execute_command(command, DEFAULT_DERICTIVE);

        free(path);
        free(command);
    }
}

void shell_start_screen() {
    printf("\n\n");

    cprintf(FOREGROUND_LIGHT_GREEN, "  .o88b.  .d88b.  d8888b. d8888b. d88888b db      db         .d88b.  .d8888. \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, " d8P  Y8 .8P  Y8. 88  `8D 88  `8D 88'     88      88        .8P  Y8. 88'  YP \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, " 8P      88    88 88oobY' 88   88 88ooooo 88      88        88    88 `8bo.   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, " 8b      88    88 88`8b   88   88 88~~~~~ 88      88        88    88   `Y8b. \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, " Y8b  d8 `8b  d8' 88 `88. 88  .8D 88.     88booo. 88booo.   `8b  d8' db   8D \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  `Y88P'  `Y88P'  88   YD Y8888D' Y88888P Y88888P Y88888P    `Y88P'  `8888Y' \r\n");

    cprintf(FOREGROUND_AQUA, "\r\n Questo sistema operativo 'e in costruzione. [ver. 0.5.7a | 03.01.2024] \r\n");
}

///////////////////////////////////////
//
//  SHELL COMMANDS
//
//

    void execute_command(char* command, int cordell_derictive) {

        ////////////////////////////////
        //
        //  PASSWORD VERIFICATION
        //

            if (cordell_derictive == CORDELL_DERICTIVE) {
                cprintf(FOREGROUND_GREEN, "\r\n[PAROLA D'ORDINE]: ");
                char* password = keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);
                int tries = 0;

                char* pass = FS_read_file(currentPassword);
                while (strcmp(password, pass) != 0) {
                    cprintf(FOREGROUND_RED, "\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    free(password);
                    free(pass);

                    password = keyboard_read(HIDDEN_KEYBOARD, FOREGROUND_WHITE);
                    if (++tries >= MAX_ATTEMPT_COUNT) 
                        return;
                }

                free(password);
                free(pass);
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

            else if (strstr(command_line[0], COMMAND_VERSION) == 0)
                shell_start_screen();

            else if (strstr(command_line[0], COMMAND_DISK_DATA) == 0) {
                printf("\r\nDisk-data utility ver 0.1\n");
                printf("SECTORS ALL: [%i]\n", ATA_global_sector_count());
                printf("FULLY-EMPTY SECTORS: [%i]\n", ATA_global_sector_empty());

                // printf("FAT TYPE: [%i]\n", fat_type);
                // printf("TOTAL CLUSTERS x32: [%i]\n", total_clusters);
            }

            else if (strstr(command_line[0], COMMAND_CLEAR) == 0) 
                VGA_clrscr();

            else if (strstr(command_line[0], COMMAND_ECHO) == 0) 
                printf("\r\n%s", command_line[1]);

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                datetime_read_rtc();
                printf("\r\nGiorno: %i/%i/%i\tTempo: %i:%i:%i", datetime_day, datetime_month, datetime_year, 
                                                                datetime_hour, datetime_minute, datetime_second);
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

            else if (strstr(command_line[0], COMMAND_CREATE_DIR) == 0)
                FS_create_directory(command_line[1]);
            
            else if (strstr(command_line[0], COMMAND_GO_TO_MANAGER) == 0)                      
                open_file_manager(user);                                             

            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0)     
                FS_move_to_directory(command_line[1], FS_get_current_directory());
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) 
                FS_up_from_directory();
            
            else if (strstr(command_line[0], COMMAND_DELETE_DIR) == 0) {
                struct Directory* directory = FS_find_directory(command_line[1], FS_global_find_directory(command_line[2]));
                if (directory->subDirectory != NULL || directory->files != NULL) 
                    if (user->edit_access = 0) FS_delete_directory(command_line[1], FS_global_find_directory(command_line[2])); 
                    else printf("\r\nDirectory non vuota.\r\n");
                else FS_delete_directory(command_line[1], FS_global_find_directory(command_line[2]));                                              
            }
            
            else if (strstr(command_line[0], COMMAND_CREATE_FILE) == 0)                
                FS_create_file(atoi(command_line[1]), atoi(command_line[2]), atoi(command_line[3]), command_line[4], command_line[5], 
                ATA_find_empty_sector(FILES_SECTOR_OFFSET));      
                             
            else if (strstr(command_line[0], COMMAND_DELETE_FILE) == 0)  {    
                struct File* file = FS_global_find_file(command_line[1]);
                if (file == NULL)
                    return;

                if (file->edit_level >= user->edit_access) FS_delete_file(command_line[1], FS_global_find_directory(command_line[2]));
                else printf("\nYou don`t have permissions to do this.");
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {                              
                struct Directory* current_dir = FS_get_current_directory()->subDirectory;
                if (current_dir != NULL) {
                    printf("\r\n\t%s", current_dir->name);
            
                    while (current_dir->next != NULL) {
                        current_dir = current_dir->next;
                        printf("\t%s", current_dir->name);
                    }
                }
            
                struct File* current = FS_get_current_directory()->files;
                if (current != NULL) {
                    printf("\r\n\t%s", current->name);
            
                    while (current->next != NULL) {
                        current = current->next;
                        printf("\t%s", current->name);
                    }
                }
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                struct File* file = FS_find_file(command_line[1], FS_get_current_directory());
                if (file == NULL)
                    return;
                
                if (file->read_level >= user->read_access) printf("\r\n%s", FS_read_file(file));
                else printf("\nYou don`t have permissions to do this.");
            }

        //
        //
        //  FILE SYSTEM COMMANDS
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        //  APPLICATIONS
        //
        //

            else if (strstr(command_line[0], COMMAND_CALCULATOR) == 0) {
                char* tokens[100];

                int tokenCount      = 0;
                char* expression    = command + strlen(command_line[0]) + 1;  
                char* splitted      = strtok(expression, " ");

                while(splitted) {
                    tokens[tokenCount++] = splitted;
                    splitted = strtok(NULL, " ");
                }

                printf("\r\n> Risposta: %s", calculator(tokens, tokenCount));
            }

            else if (strstr(command_line[0], COMMAND_SNAKE_GAME) == 0) {
                VGA_text_clrscr();

                struct File* snake_save;
                if (FS_file_exist("/snake-save") == 1)
                    snake_save = FS_global_find_file("/snake-save");
                else {
                    FS_create_file(0, 0, 0, "/snake-save", "obj", ATA_find_empty_sector(FILES_SECTOR_OFFSET));
                    snake_save = FS_global_find_file("/snake-save");
                    FS_write_file(snake_save, "0");
                }

                char* file_data = FS_read_file(snake_save);
                int best_result = atoi(file_data);
                free(file_data);
                
                int current_result = snake_init(atoi(command_line[1]), best_result);
                if (best_result < current_result) {
                    char* result = itoa(current_result);
                    FS_write_file(snake_save, result);

                    free(result);
                }
            }

            else if (strstr(command_line[0], COMMAND_TETRIS_GAME) == 0) {
                VGA_text_clrscr();

                struct File* tetris_save;
                if (FS_file_exist("/tetris-save") == 1)
                    tetris_save = FS_global_find_file("/tetris-save");
                else {
                    FS_create_file(0, 0, 0, "/tetris-save", "obj", ATA_find_empty_sector(FILES_SECTOR_OFFSET));
                    tetris_save = FS_global_find_file("/tetris-save");
                    FS_write_file(tetris_save, "0");
                }

                char* file_data = FS_read_file(tetris_save);
                int best_result = atoi(file_data);
                free(file_data);
                
                int current_result = init_tetris(best_result);
                if (best_result < current_result) {
                    char* result = itoa(current_result);
                    FS_write_file(tetris_save, result);

                    free(result);
                }
            }

            else if (strstr(command_line[0], COMMAND_FILE_EDIT) == 0) {
                struct File* file = FS_find_file(command_line[1], FS_get_main_directory());
                if (file == NULL)
                    return;
                
                if (file->write_level >= user->write_access) 
                    text_editor_init(file, FOREGROUND_WHITE + BACKGROUND_BLACK);
                else printf("\nYou don`t have permissions to do this.");
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                struct File* execute = FS_find_file(command_line[1], FS_get_main_directory());
                if (execute == NULL)
                    return;
                
                if (execute->edit_level >= user->edit_access) {
                    char* sector_data = FS_read_file(execute);
                    char* command_for_split = (char*)malloc(strlen(sector_data));
                    strcpy(command_for_split, sector_data);

                    char* lines[100];
                    int tokenCount = 0;

                    char* splitted = strtok(command_for_split, "\n");
                    while(splitted) {
                        lines[tokenCount++] = splitted;
                        splitted = strtok(NULL, "\n");
                    }

                    for (int i = 0; i < tokenCount; i++)
                        if (cordell_derictive == CORDELL_DERICTIVE) execute_command(lines[i], SUPER_DERICTIVE);
                        else execute_command(lines[i], DEFAULT_DERICTIVE);

                    free(command_for_split);
                    free(sector_data);
                }
                else printf("\nYou don`t have permissions to do this.");
            }

            else if (strstr(command_line[0], COMMAND_FILE_ASM_RUN) == 0) {
                struct File* execute = FS_find_file(command_line[1], FS_get_main_directory());
                if (execute == NULL)
                    return;

                if (execute->edit_level >= user->edit_access) {
                    char* sector_data = FS_read_file(execute);
                    asm_execute(sector_data, user);

                    free(sector_data);
                }
                else printf("\nYou don`t have permissions to do this.");
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
