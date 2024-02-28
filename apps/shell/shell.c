#include "shell.h"


char* current_path = "BOOT";
char* command_buffer[COMMAND_BUFFER] = { NULL };
int exit = 0;
int current_command = 0;


void main(int args, char* argv[]) {
    clrscr();
    shell_start_screen();

    while (1) {
        printf("\n[KERNEL] $%s> ", current_path);
        
        char stop_chars[3] = { ENTER_BUTTON, UP_ARROW_BUTTON, '\0' };
        char* input = input_read_stop(VISIBLE_KEYBOARD, FOREGROUND_WHITE, stop_chars);
        char* command = input;

        if (input[max(0, strlen(input) - 1)] == UP_ARROW_BUTTON) {
            char* saved_command = command_buffer[max(0, --current_command)];
            if (saved_command != NULL) {
                command = saved_command;
                printf("%s", saved_command);
            }

            if (current_command < 0) current_command = 0;
        }

        if (current_command >= COMMAND_BUFFER) {
            free(command_buffer[0]);
            for (int i = 1; i < COMMAND_BUFFER; i++) 
                command_buffer[i - 1] = command_buffer[i];
            
            current_command = COMMAND_BUFFER - 1;
        }
        
        command_buffer[current_command++] = input;
        execute_command(command);
    }

    free(current_path);
    tkill();
}

void shell_start_screen() {
    printf("Cordell KShell [ver. 0.4a | 28.02.2024]\n");
    printf("Stai entrando nella shell del kernel leggero. Usa [aiuto] per ottenere aiuto.\n\n");
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
                printf("\n> Usa [%s] per ottenere aiuto",                                     COMMAND_HELP);
                printf("\n> Utilizzare [%s] per la pulizia dello schermo",                    COMMAND_CLEAR);
                printf("\n> Usa [%s] per l'eco",                                              COMMAND_ECHO);
                printf("\n> Usa [%s] per vista questo giorno\n",                              COMMAND_TIME);
                printf("\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                printf("\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);
                printf("\n> Utilizza la [%s] per vista mem-data informazione",                COMMAND_MEM_DATA);
                printf("\n> Utilizza la [%s] <option> per vista page-data informazione\n",    COMMAND_PAGE_DATA);
                printf("\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
                printf("\n> Usa [%s] per uscire di dir",                                      COMMAND_OUT_DIR);
                printf("\n> Usa [%s] per guardare tutto cosa in dir\n",                       COMMAND_LIST_DIR);
                printf("\n> Usa [%s] per guardare tutto data in file",                        COMMAND_FILE_VIEW);
                printf("\n> Usa [%s] per guardare bmp",                                       COMMAND_BMP_SHOW);
                printf("\n> Usa [%s] per run file\n",                                         COMMAND_FILE_RUN);
                printf("\n> Utilizzare [%s] per riavviare il sistema operativo",              COMMAND_REBOOT);
                printf("\n> Utilizzare [%s] per uscire dal kernel\n",                         COMMAND_EXIT);
            }

            else if (strstr(command_line[0], COMMAND_EXIT)      == 0) exit = 1;
            else if (strstr(command_line[0], COMMAND_REBOOT)    == 0) machine_restart();
            else if (strstr(command_line[0], COMMAND_VERSION)   == 0) shell_start_screen();
            else if (strstr(command_line[0], COMMAND_ECHO)      == 0) printf("\n%s", command_line[1]);
            // else if (strstr(command_line[0], COMMAND_MEM_DATA)  == 0) print_kmalloc_map();
            // else if (strstr(command_line[0], COMMAND_PAGE_DATA) == 0) print_page_map(command_line[1][0]);
            else if (strstr(command_line[0], COMMAND_CLEAR)     == 0) clrscr();
                
            else if (strstr(command_line[0], COMMAND_DISK_DATA) == 0) {
                uint32_t fs_data[8];
                get_fs_info(fs_data);

                printf("\nDISK-DATA KERNEL UTILITY VER 0.2c\n");
                printf("DEV:                  [%s]\n", (char*)fs_data[0]);
                printf("FS TYPE:              [%s]\n", (char*)fs_data[1]);
                printf("FAT TYPE:             [%i]\n", fs_data[2]);
                printf("TOTAL CLUSTERS x32:   [%i]\n", fs_data[3]);
                printf("TOTAL SECTORS x32:    [%i]\n", fs_data[4]);
                printf("BYTES PER SECTOR x32: [%i]\n", fs_data[5]);
                printf("SECTORS PER CLUSTER:  [%i]\n", fs_data[6]);
                printf("FAT TABLE SIZE:       [%i]\n", fs_data[7]);
            }

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                short time[6];
                get_datetime(time);
                printf("\nGiorno: %i/%i/%i\tTempo: %i:%i:%i", time[3], time[4], time[5], 
                                                                time[2], time[1], time[0]);
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
                    printf("\nLa directory non esiste.");
                    return;
                }

                struct UFATContent* content = get_content(dir_path);
                if (content->file != NULL) {
                    FATLIB_unload_content_system(content);
                    free(dir_path);
                    printf("\nQuesta non e' una directory.");
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
                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    printf("\nLa file non esiste.");
                    free(file_path);
                    return;
                }
                
                printf("\n");

                struct UFATContent* content = get_content(file_path);
                int data_size = 0;
                while (data_size < content->file->file_meta.file_size) {
                    int copy_size = min(content->file->file_meta.file_size - data_size, 128);
                    char* data    = (char*)calloc(copy_size, 1);

                    fread_off(content, data_size, data, copy_size);
                    printf("%s", data);

                    free(data);
                    data_size += copy_size;
                }
                
                FATLIB_unload_content_system(content);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_BMP_SHOW) == 0) {
                char* file_path = FATLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    printf("\nLa file non esiste.");
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
                    printf("\nLa file non esiste.");
                    free(exec_path);
                    return;
                }

                printf("\nCODE: [%i]\n", fexec(exec_path, command_line[2], command_line[3]));
                free(exec_path);
            }

        //====================
        //  FILE SYSTEM COMMANDS
        //====================
        //  NETWORKING COMMANDS
        //====================

            else if (strstr(command_line[0], COMMAND_IPCONFIG) == 0) {
                uint8_t ip[4]  = { 0x00, 0x00, 0x00, 0x00 };
                uint8_t mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

                get_ip(ip);
                get_mac(mac);

                printf("\nIPCONF KERNEL UTILITY VER 0.2a\n");
                printf("\nCURRENT IP: %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
                printf("\nCURRENT MAC: %x.%x.%x.%x.%x.%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }

            else if (strstr(command_line[0], COMMAND_SEND_PACKET) == 0) {
                uint8_t ip[4] = { 0x00, 0x00, 0x00, 0x00 };
                get_ip(ip);

                uint8_t dst_ip[4] = { atoi(command_line[1]), atoi(command_line[2]), atoi(command_line[3]), atoi(command_line[4]) };
                uint16_t dst_port = atoi(command_line[5]);
                uint16_t src_port = atoi(command_line[6]);

                printf("\nTRANSFERING [%s] FROM [%i.%i.%i.%i:%i] TO [%i.%i.%i.%i:%i]",
                                                command_line[7], ip[0], ip[1], ip[2], ip[3], src_port,
                                                dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3], dst_port);
                send_udp_packet(dst_ip, src_port, dst_port, command_line[7], strlen(command_line[7]));
            }

            else if (strstr(command_line[0], COMMAND_POP_PACKET) == 0) {
                uint8_t buffer[512];
                pop_received_udp_packet(buffer);

                printf("\n");
                printf("RECEIVED UDP PACKET STR:       [%s]\n", (char*)buffer);
                printf("RECEIVED UDP PACKET UINT:      [%u]\n", buffer);
                printf("RECEIVED UDP PACKET INT:       [%i]\n", buffer);
                printf("RECEIVED UDP PACKET HEX:       [%x]\n", buffer);
                printf("RECEIVED UDP PACKET POINTER:   [%p]\n", buffer);
            }

        //====================
        //  NETWORKING COMMANDS
        //====================

            else printf("\nComando [%s] sconosciuto.", command_line[0]);

        printf("\n");
    }

//====================
//  KSHELL COMMANDS
//====================