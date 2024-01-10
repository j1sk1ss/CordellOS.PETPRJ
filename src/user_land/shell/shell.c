#include "include/shell.h"
// TODO: fat.c
//   ____  _   _ _____ _     _     
//  / ___|| | | | ____| |   | |    
//  \___ \| |_| |  _| | |   | |    
//   ___) |  _  | |___| |___| |___ 
//  |____/|_| |_|_____|_____|_____|
                                

struct User* user = NULL;

void shell() {

    shell_start_screen();
    init_users();
    
    /////////////
    //  SHELL CORDELL PASSWORD
    //

        if (FAT_content_exists("boot\\shell.txt") == 0) {
            struct FATContent* content = FAT_create_content("shell", FALSE, "txt");
            FAT_put_content("boot", content);
            FAT_edit_content("boot\\shell.txt", "12345");

            FAT_unload_content_system(content);
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
        cprintf(FOREGROUND_GREEN, "\r\n[%s: CORDELL OS]", user->name);
        printf(" $%s> ", FAT_get_current_path());

        char* command = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        if (strstr(command, "cordell") == 0)
            execute_command(command + strlen("cordell") + 1, CORDELL_DERICTIVE);
        else
            execute_command(command, DEFAULT_DERICTIVE);

        free(command);
    }
}

void shell_start_screen() {
    printf("\n\n");

    cprintf(FOREGROUND_LIGHT_GREEN, "     .aMMMb  .aMMMb  dMMMMb  dMMMMb  dMMMMMP dMP     dMP        .aMMMb  .dMMMb \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "    dMP'VMP dMP'dMP dMP.dMP dMP VMP dMP     dMP     dMP        dMP'dMP dMP' VP \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "   dMP     dMP dMP dMMMMK' dMP dMP dMMMP   dMP     dMP        dMP dMP  VMMMb   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  dMP.aMP dMP.aMP dMP'AMF dMP.aMP dMP     dMP     dMP        dMP.aMP dP .dMP   \r\n");
    cprintf(FOREGROUND_LIGHT_GREEN, "  VMMMP'  VMMMP' dMP dMP dMMMMP' dMMMMMP dMMMMMP dMMMMMP     VMMMP'  VMMMP'    \r\n");

    cprintf(FOREGROUND_LIGHT_AQUA, "\r\n\t Questo sistema operativo 'e in costruzione. [ver. 1.0.0a | 09.01.2024] \r\n");
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

                char* pass = FAT_get_content("boot\\shell.txt")->file->data;
                while (strcmp(password, pass) != 0) {
                    cprintf(FOREGROUND_RED, "\r\nPassword errata, riprova.\r\n[PAROLA D'ORDINE]: ");
                    free(password);

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

            else if (strstr(command_line[0], COMMAND_CREATE_DIR) == 0) {
                struct FATContent* content = FAT_create_content(command_line[1], TRUE, NULL);
                FAT_put_content(FAT_get_current_path(), content);

                FAT_unload_content_system(content);
            }

            else if (strstr(command_line[0], COMMAND_GO_TO_MANAGER) == 0)                      
                open_file_manager(user);                                             

            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                if (FAT_content_exists(FAT_get_current_path()) == 0) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("\nDirectory not exists.");
                }

                else if (FAT_get_content(FAT_get_current_path())->file != NULL) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("\nThis is no a Directory.");
                }
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                if (strlen(FAT_get_current_path()) <= 1)
                    FAT_set_current_path("BOOT");
            }
            
            else if (strstr(command_line[0], COMMAND_DELETE_DIR) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                if (FAT_content_exists(FAT_get_current_path()) == 0) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("Content not exists.\n");
                    return;
                } else if (FAT_get_content(FAT_get_current_path())->directory == NULL) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("Content is not a directory exists.\n");
                    return;
                }

                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                FAT_delete_content(FAT_get_current_path(), command_line[1]);
            }
            
            else if (strstr(command_line[0], COMMAND_CREATE_FILE) == 0) {
                struct FATContent* content = FAT_create_content(command_line[1], FALSE, command_line[2]);
                FAT_put_content(FAT_get_current_path(), content);

                FAT_unload_content_system(content);
            }

            else if (strstr(command_line[0], COMMAND_DELETE_FILE) == 0)  {    
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                if (FAT_content_exists(FAT_get_current_path()) == 0) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("Content not exists.\n");
                    return;
                } else if (FAT_get_content(FAT_get_current_path())->file == NULL) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    printf("Content is not a directory exists.\n");
                    return;
                }

                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                FAT_delete_content(FAT_get_current_path(), command_line[1]);
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct FATContent* content = FAT_get_content(FAT_get_current_path());
                if (content->directory != NULL) {
                    struct FATDirectory* directory = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE);
                    struct FATDirectory* current_dir = directory->subDirectory;
                    struct FATFile* current_file = directory->files;

                    if (current_dir != NULL) {
                        printf("\r\n\t%s", current_dir->name);
                
                        while (current_dir->next != NULL) {
                            current_dir = current_dir->next;
                            printf("\t%s", current_dir->name);
                        }
                    }

                    if (current_file != NULL) {
                        printf("\r\n\t%s", current_file->name);
                
                        while (current_file->next != NULL) {
                            current_file = current_file->next;
                            printf("\t%s.%s", current_file->name, current_file->extension);
                        }
                    }

                    FAT_unload_files_system(current_file);
                    FAT_unload_directories_system(current_dir);
                    FAT_unload_directories_system(directory);
                }

                FAT_unload_content_system(content);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                struct FATContent* content = FAT_get_content(FAT_get_current_path());
                struct FATFile* file = content->file;
                if (file == NULL) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    return;
                }
                
                printf("\r\n%s", file->data);
                FAT_unload_content_system(content);
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
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

            else if (strstr(command_line[0], COMMAND_FILE_EDIT) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                text_editor_init(FAT_get_current_path(), BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                struct FATContent* content = FAT_get_content(command_line[1]);
                if (content->file == NULL)
                    return;

                char* sector_data = content->file->data;
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
                FAT_unload_content_system(content);
            }

            else if (strstr(command_line[0], COMMAND_FILE_ASM_RUN) == 0) {
                struct FATContent* content = FAT_get_content(command_line[1]);
                if (content->file == NULL)
                    return;

                char* sector_data = content->file->data;
                asm_execute(sector_data, user);

                free(sector_data);
                FAT_unload_content_system(content);
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
