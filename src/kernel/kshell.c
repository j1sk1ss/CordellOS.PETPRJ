#include "include/kshell.h"

int exit = 0;

void kshell() {
    shell_start_screen();
    while (exit == 0) {
        kprintf("\r\n[KERNEL] $%s> ", FAT_get_current_path());
        char* command = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        execute_command(command);
        free(command);
    }
}

void shell_start_screen() {
    kprintf("Cordell Kernel [ver. 0.2d | 25.01.2024] \n\r");
    kprintf("You are entering to light kernel shell. Use [aiuto] for getting help. \n\n\r");
}

//====================
//  KSHELL COMMANDS
//====================

    void execute_command(char* command) {

        //====================
        //  SPLIT COMMAND LINE TO ARGS
        //====================

            char* command_line[100];
            int tokenCount = 0;

            char* command_for_split = (char*)malloc(strlen(command));
            strcpy(command_for_split, command);

            char* splitted = strtok(command_for_split, " ");
            while(splitted) {
                command_line[tokenCount++]  = splitted;
                splitted                    = strtok(NULL, " ");
            }

        //====================
        //  SPLIT COMMAND LINE TO ARGS
        //====================
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //====================

            if (strstr(command_line[0], COMMAND_HELP) == 0) {
                kprintf("\r\n> Usa [%s] per ottenere aiuto",                                     COMMAND_HELP);
                kprintf("\r\n> Utilizzare [%s] per la pulizia dello schermo",                    COMMAND_CLEAR);
                kprintf("\r\n> Usa [%s] per l'eco",                                              COMMAND_ECHO);
                kprintf("\r\n> Usa [%s] per vista questo giorno",                                COMMAND_TIME);

                kprintf("\r\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                kprintf("\r\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);

                kprintf("\r\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                kprintf("\r\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                kprintf("\r\n> Usa [%s] per guardare tutto cosa in dir",                         COMMAND_LIST_DIR);

                kprintf("\r\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                kprintf("\r\n> Usa [%s] per run file",                                           COMMAND_FILE_RUN);

                kprintf("\r\n> Usa [%s] per uscire di kernel shell",                             COMMAND_EXIT);
                kprintf("\n");
            }

            else if (strstr(command_line[0], COMMAND_EXIT) == 0) exit = 1;
            else if (strstr(command_line[0], COMMAND_VERSION) == 0) shell_start_screen();

            else if (strstr(command_line[0], COMMAND_DISK_DATA) == 0) {
                kprintf("\r\nDisk-data kernel utility ver 0.2b\n");
                
                kprintf("FAT TYPE:             [%i]\n", fat_type);
                kprintf("TOTAL CLUSTERS x32:   [%i]\n", total_clusters);
                kprintf("TOTAL SECTORS x32:    [%i]\n", total_sectors);
                kprintf("BYTES PER SECTOR x32: [%i]\n", bytes_per_sector);
                kprintf("SECTORS PER CLUSTER:  [%i]\n", sectors_per_cluster);
                kprintf("FAT TABLE SIZE:       [%i]\n", fat_size);
                kprintf("\n");
            }

            else if (strstr(command_line[0], COMMAND_CLEAR) == 0) VGA_clrscr();
            else if (strstr(command_line[0], COMMAND_ECHO) == 0) kprintf("\r\n%s", command_line[1]);

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                datetime_read_rtc();
                kprintf("\r\nGiorno: %i/%i/%i\tTempo: %i:%i:%i", datetime_day, datetime_month, datetime_year, 
                                                                datetime_hour, datetime_minute, datetime_second);
            }

        //====================
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //====================
        //  FILE SYSTEM COMMANDS
        //====================

            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0) {
                str_uppercase(command_line[1]);
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                if (FAT_content_exists(FAT_get_current_path()) == 0) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    kprintf("\nDirectory not exists.");
                }

                else if (FAT_get_content(FAT_get_current_path())->file != NULL) {
                    FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                    kprintf("\nThis is no a Directory.");
                }
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                if (strlen(FAT_get_current_path()) <= 1) FAT_set_current_path("BOOT");
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct FATContent* content = FAT_get_content(FAT_get_current_path());
                if (content->directory != NULL) {
                    struct FATDirectory* directory = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE);
                    struct FATDirectory* current_dir = directory->subDirectory;
                    struct FATFile* current_file = directory->files;

                    if (current_dir != NULL) {
                        kprintf("\r\n\t%s", current_dir->name);
                
                        while (current_dir->next != NULL) {
                            current_dir = current_dir->next;
                            kprintf("\t%s", current_dir->name);
                        }
                    }

                    if (current_file != NULL) {
                        kprintf("\r\n\t%s.%s", current_file->name, current_file->extension);
                
                        while (current_file->next != NULL) {
                            current_file = current_file->next;
                            kprintf("\t%s.%s", current_file->name, current_file->extension);
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
                
                char* data = FAT_read_content(content);
                kprintf("\r\n%s", data);
                free(data);

                FAT_unload_content_system(content);
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), command_line[1]));
                kprintf("CODE: [%i]\n", FAT_ELF_execute_content(FAT_get_current_path(), command_line[2], command_line[3]));
                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
            }

        //====================
        //  FILE SYSTEM COMMANDS
        //====================

            else 
                kprintf("\r\nComando sconosciuto.");

        free(command_for_split);

        kprintf("\r\n");
    }

//====================
//  KSHELL COMMANDS
//====================