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
        char* input   = keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE);
        char* command = input;

        execute_command(command);
        free(input);
    }

    free(current_path);
    tkill();
}

void shell_start_screen() {
    kprintf("Cordell KShell [ver. 0.3g | 26.02.2024] \n\r");
    kprintf("Stai entrando nella shell del kernel leggero. Usa [aiuto] per ottenere aiuto.\n\n\r");
}

//====================
//  KSHELL COMMANDS
//====================

    void execute_command(char* command) {

        if (command == NULL) return;
        if (strlen(command) <= 0) return;

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
                kprintf("\r\n> Usa [%s] per vista questo giorno\n",                              COMMAND_TIME);
                kprintf("\r\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                kprintf("\r\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);
                kprintf("\r\n> Utilizza la [%s] per vista mem-data informazione",                COMMAND_MEM_DATA);
                kprintf("\r\n> Utilizza la [%s] <option> per vista page-data informazione\n",    COMMAND_PAGE_DATA);
                kprintf("\r\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                kprintf("\r\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                kprintf("\r\n> Usa [%s] per guardare tutto cosa in dir\n",                       COMMAND_LIST_DIR);
                kprintf("\r\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                if (is_vesa) kprintf("\r\n> Usa [%s] per guardare bmp",                          COMMAND_BMP_SHOW);
                kprintf("\r\n> Usa [%s] per run file\n",                                         COMMAND_FILE_RUN);
                kprintf("\r\n> Utilizzare [%s] per riavviare il sistema operativo",              COMMAND_REBOOT);
                kprintf("\r\n> Utilizzare [%s] per uscire dal kernel\n",                         COMMAND_EXIT);
            }

            else if (strstr(command_line[0], COMMAND_EXIT)      == 0) exit = 1;
            else if (strstr(command_line[0], COMMAND_REBOOT)    == 0) i386_reboot();
            else if (strstr(command_line[0], COMMAND_VERSION)   == 0) shell_start_screen();
            else if (strstr(command_line[0], COMMAND_ECHO)      == 0) kprintf("\r\n%s", command_line[1]);
            else if (strstr(command_line[0], COMMAND_MEM_DATA)  == 0) print_kmalloc_map();
            else if (strstr(command_line[0], COMMAND_PAGE_DATA) == 0) print_page_map(command_line[1][0]);
            else if (strstr(command_line[0], COMMAND_CLEAR)     == 0) kclrscr();
                
            else if (strstr(command_line[0], COMMAND_DISK_DATA) == 0) {
                kprintf("\r\nDISK-DATA KERNEL UTILITY VER 0.2c\n");
                kprintf("DEV:                  [%s]\n", current_vfs->device->mountpoint);
                kprintf("FS TYPE:              [%s]\n\n", current_vfs->name);
                kprintf("FAT TYPE:             [%i]\n", fat_type);
                kprintf("TOTAL CLUSTERS x32:   [%i]\n", total_clusters);
                kprintf("TOTAL SECTORS x32:    [%i]\n", total_sectors);
                kprintf("BYTES PER SECTOR x32: [%i]\n", bytes_per_sector);
                kprintf("SECTORS PER CLUSTER:  [%i]\n", sectors_per_cluster);
                kprintf("FAT TABLE SIZE:       [%i]\n", fat_size);
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
                if (cexists(current_path) == 0) {
                    free(dir_path);
                    kprintf("\nLa directory non esiste.");
                    return;
                }

                struct UFATContent* content = get_content(dir_path);
                if (content->file != NULL) {
                    FATLIB_unload_content_system(content);
                    free(dir_path);
                    kprintf("\nQuesta non e' una directory.");
                    return;
                }

                FATLIB_unload_content_system(content);
                free(current_path);
                current_path = dir_path;
            }
            
            else if (strstr(command_line[0], COMMAND_OUT_DIR) == 0) {
                char* up_path = FATLIB_change_path(current_path, NULL);
                if (up_path == NULL) {
                    up_path = malloc(5);
                    strcpy(up_path, "BOOT");
                }

                free(current_path);
                current_path = up_path;
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                struct UFATDirectory* directory   = opendir(current_path);
                struct UFATDirectory* current_dir = directory->subDirectory;
                struct UFATFile* current_file     = directory->files;

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
                
                FATLIB_unload_directories_system(directory);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(file_path);
                    return;
                }
                
                kprintf("\n");

                struct UFATContent* content = get_content(file_path);
                int data_size = 0;
                while (data_size < content->file->file_meta.file_size) {
                    int copy_size = min(content->file->file_meta.file_size - data_size, 128);
                    char* data    = (char*)calloc(copy_size, 1);

                    fread_off(content, data_size, data, copy_size);
                    kprintf("%s", data);

                    free(data);
                    data_size += copy_size;
                }
                
                FATLIB_unload_content_system(content);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_BMP_SHOW) == 0) {
                if (!is_vesa) return;

                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(file_path);
                    return;
                }
                
                struct bitmap* bitmap = BMP_create(file_path, atoi(command_line[2]), atoi(command_line[3]));
                
                BMP_display(bitmap);
                BMP_unload(bitmap);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                char* exec_path = FATLIB_change_path(current_path, command_line[1]);
                if (cexists(exec_path) == 0) {
                    kprintf("\nLa file non esiste.");
                    free(exec_path);
                    return;
                }

                kprintf("\nCODE: [%i]\n", fexec(exec_path, command_line[2], command_line[3]));
                free(exec_path);
            }

        //====================
        //  FILE SYSTEM COMMANDS
        //====================
        //  NETWORKING COMMANDS
        //====================

            else if (strstr(command_line[0], COMMAND_IPCONFIG) == 0) {
                uint8_t ip[4];
                int allocated = DHCP_get_host_addr(ip);

                uint8_t mac[6];
                get_mac_addr(mac);

                kprintf("\r\nIPCONF KERNEL UTILITY VER 0.1a\n");
                kprintf("\nCURRENT [%s] IP: %i.%i.%i.%i", allocated == 0 ? "STATIC" : "DYNAMIC",
                                                          ip[0], ip[1], ip[2], ip[3]);
                kprintf("\nCURRENT MAC: %x.%x.%x.%x.%x.%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }

            else if (strstr(command_line[0], COMMAND_SEND_PACKET) == 0) {
                uint8_t ip[4];
                DHCP_get_host_addr(ip);

                uint8_t dst_ip[4] = { atoi(command_line[1]), atoi(command_line[2]), atoi(command_line[3]), atoi(command_line[4]) };
                uint16_t dst_port = atoi(command_line[5]);
                uint16_t src_port = atoi(command_line[6]);

                kprintf("\nTRANSFERING [%s] FROM [%i.%i.%i.%i:%i] TO [%i.%i.%i.%i:%i]",
                                                command_line[7], ip[0], ip[1], ip[2], ip[3], src_port,
                                                dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3], dst_port);
                UDP_send_packet(dst_ip, src_port, dst_port, command_line[7], strlen(command_line[7]));
            }

            else if (strstr(command_line[0], COMMAND_POP_PACKET) == 0) {
                void* data = UDP_pop_packet();
                if (data != NULL) {
                    kprintf("\n");
                    kprintf("RECEIVED UDP PACKET STR:       [%s]\n", data);
                    kprintf("RECEIVED UDP PACKET UINT:      [%u]\n", data);
                    kprintf("RECEIVED UDP PACKET INT:       [%i]\n", data);
                    kprintf("RECEIVED UDP PACKET HEX:       [%x]\n", data);
                    kprintf("RECEIVED UDP PACKET POINTER:   [%p]\n", data);
                    free(data);
                }
            }

        //====================
        //  NETWORKING COMMANDS
        //====================

            else kprintf("\r\nComando [%s] sconosciuto.", command_line[0]);

        kprintf("\r\n");
    }

//====================
//  KSHELL COMMANDS
//====================