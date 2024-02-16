#include "include/kshell.h"

int exit = 0;
char* current_path = NULL;

void kshell() {
    if (!is_vesa) kprintf("[NOTA: QUESTA E` UNA MODALITA` TESTO VGA]\n");
    current_path = (char*)malloc(5);
	strcpy(current_path, "BOOT");

    shell_start_screen();
    while (exit == 0) {
        kprintf("\r\n[KERNEL] $%s> ", current_path);
        char* input = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        char* command = input;

        execute_command(command);
        free(input);
    }

    free(current_path);
    tkill();
}

void shell_start_screen() {
    kprintf("Cordell KShell [ver. 0.3c | 15.02.2024] \n\r");
    kprintf("Stai entrando nella shell del kernel leggero. Usa [aiuto] per ottenere aiuto.\n\n\r");
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

            char* splitted = strtok(command, " ");
            while(splitted) {
                command_line[tokenCount++] = splitted;
                splitted                   = strtok(NULL, " ");
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
                kprintf("\n");

                kprintf("\r\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                kprintf("\r\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);
                kprintf("\n");

                kprintf("\r\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                kprintf("\r\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                kprintf("\r\n> Usa [%s] per guardare tutto cosa in dir",                         COMMAND_LIST_DIR);
                kprintf("\n");

                kprintf("\r\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                kprintf("\r\n> Usa [%s] per guardare bmp",                                       COMMAND_BMP_SHOW);
                kprintf("\r\n> Usa [%s] per run file",                                           COMMAND_FILE_RUN);
                kprintf("\n");

                kprintf("\r\n> Utilizzare [%s] per riavviare il sistema operativo",              COMMAND_REBOOT);
                kprintf("\r\n> Utilizzare [%s] per uscire dal kernel",                           COMMAND_EXIT);
                kprintf("\n");
            }

            else if (strstr(command_line[0], COMMAND_EXIT)     == 0) exit = 1;
            else if (strstr(command_line[0], COMMAND_REBOOT)   == 0) i386_reboot();
            else if (strstr(command_line[0], COMMAND_VERSION)  == 0) shell_start_screen();
            else if (strstr(command_line[0], COMMAND_ECHO)     == 0) kprintf("\r\n%s", command_line[1]);
            else if (strstr(command_line[0], COMMAND_MEM_DATA) == 0) print_kmalloc_map();
            else if (strstr(command_line[0], COMMAND_CLEAR)    == 0) 
                if (!is_vesa) VGA_clrscr();
                else VESA_clrscr();
                
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
                char* dir_path = FATLIB_change_path(current_path, command_line[1]);
                if (FAT_content_exists(current_path) == 0) {
                    free(dir_path);
                    kprintf("\nLa directory non esiste.");
                    return;
                }

                struct FATContent* content = FAT_get_content(dir_path);
                if (content->file != NULL) {
                    FAT_unload_content_system(content);
                    free(dir_path);
                    kprintf("\nQuesta non e' una directory.");
                    return;
                }

                FAT_unload_content_system(content);
                free(current_path);
                current_path = dir_path;
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                char* up_path = FATLIB_change_path(current_path, NULL);
                if (up_path == NULL) {
                    up_path = malloc(5);
                    strcpy(up_path, "BOOT");
                };

                free(current_path);
                current_path = up_path;
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct FATContent* content = FAT_get_content(current_path);
                if (content->directory != NULL) {
                    struct FATDirectory* directory   = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE);
                    struct FATDirectory* current_dir = directory->subDirectory;
                    struct FATFile* current_file     = directory->files;

                    kprintf("\n");
                    while (current_dir != NULL) {
                        kprintf("\t%s", current_dir->name);
                        current_dir = current_dir->next;
                    }

                    kprintf("\n");
                    while (current_file != NULL) {
                        kprintf("\t%s.%s", current_file->name, current_file->extension);
                        current_file = current_file->next;
                    }
                    
                    FAT_unload_directories_system(directory);
                }
                
                FAT_unload_content_system(content);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (FAT_content_exists(file_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(file_path);
                    return;
                }
                
                kprintf("\n");

                struct FATContent* content = FAT_get_content(file_path);
                int data_size = 0;
                while (data_size < content->file->file_meta.file_size) {
                    int copy_size = min(content->file->file_meta.file_size - data_size, 128);
                    char* data    = (char*)malloc(copy_size);
                    FAT_read_content2buffer(content, data, data_size, copy_size);

                    kprintf("%s");

                    free(data);
                    data_size += copy_size;
                }
                
                FAT_unload_content_system(content);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_BMP_SHOW) == 0) {
                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (FAT_content_exists(file_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(file_path);
                    return;
                }

                struct bitmap* bitmap = BMP_create(file_path);
                BMP_display_at(bitmap, atoi(command_line[2]), atoi(command_line[3]));
                BMP_unload(bitmap);

                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                char* exec_path = FATLIB_change_path(current_path, command_line[1]);
                if (FAT_content_exists(exec_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(exec_path);
                    return;
                }

                kprintf("\nCODE: [%i]\n", FAT_ELF_execute_content(exec_path, command_line[2], command_line[3]));
                free(exec_path);
            }

        //====================
        //  FILE SYSTEM COMMANDS
        //====================

            else kprintf("\r\nComando [%s] sconosciuto.", command_line[0]);

        kprintf("\r\n");
    }

//====================
//  KSHELL COMMANDS
//====================