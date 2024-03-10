#include "shell.h"


char* current_path = "HOME";
int exit = 1;
int current_command = 0;


void main(int argc, char* argv[]) {
    clrscr();
    shell_start_screen();

    while (exit) {
        printf("\n$%s> ", current_path);
        
        char stop_chars[3] = { ENTER_BUTTON, '\0' };
        char input[COMMAND_LENGHT] = { '\0' };
        keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE, stop_chars, input);
        
        int last_char = max(0, strlen(input) - 1);
        input[last_char] = '\0';
        
        execute_command(input);
    }

    free(current_path);
    tkill();
}

void shell_start_screen() {
    printf("\n");
    printf("Cordell Shell [ver. 0.5a | 09.03.2024]\n");
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
            for (int i = 0; i < 100; i++) 
                command_line[i] = NULL;

            int tokenCount = 0;
            char* splitted = strtok(command, " ");
            while (splitted) {
                command_line[tokenCount++] = splitted;
                splitted = strtok(NULL, " ");
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
                printf("\n> Usa [%s] per ottenere informazioni sulla configurazione di rete", COMMAND_IPCONFIG);
                printf("\n> Usa [%s] per iniziare a inviare pacchetti UDP",                   COMMAND_SEND_UDP_PACKET);
                printf("\n> Usa [%s] per ottenere l'ultimo pacchetto UDP ricevuto\n",         COMMAND_POP_UDP_PACKET);
                printf("\n> Usa [%s] per ottenere informazioni sul contenuto di fs",          COMMAND_CINFO);
                printf("\n> Utilizza la [%s] per vista versione",                             COMMAND_VERSION);
                printf("\n> Utilizza la [%s] per vista disk-data informazione",               COMMAND_DISK_DATA);
                printf("\n> Utilizza la [%s] per vista mem-data informazione",                COMMAND_MEM_DATA);
                printf("\n> Utilizza la [%s] <option> per vista page-data informazione\n",    COMMAND_PAGE_DATA);
                printf("\n> Usa [%s] <nome> per entranto dir",                                COMMAND_IN_DIR);
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
            else if (strstr(command_line[0], COMMAND_MEM_DATA)  == 0) print_malloc_map();
            else if (strstr(command_line[0], COMMAND_PAGE_DATA) == 0) print_page_map(command_line[1][0]);
            else if (strstr(command_line[0], COMMAND_CLEAR)     == 0) clrscr();
                
            else if (strstr(command_line[0], COMMAND_DISK_DATA) == 0) {
                uint32_t fs_data[8];
                get_fs_info(fs_data);

                printf("\nUTILITY KERNEL DISCO-DATI VER 0.2c\n");
                printf("DEV:                             [%s]\n", (char*)fs_data[0]);
                printf("FS TYPE:                         [%s]\n", (char*)fs_data[1]);
                printf("FAT TYPE:                        [%i]\n", fs_data[2]);
                printf("CLUSTER TOTALI x32:              [%i]\n", fs_data[3]);
                printf("TOTALE SETTORI x32:              [%i]\n", fs_data[4]);
                printf("BYTE PER SETTORE x32:            [%i]\n", fs_data[5]);
                printf("SETTORI PER CLUSTER:             [%i]\n", fs_data[6]);
                printf("DIMENSIONE DELLA TABELLA GRASSO: [%i]\n", fs_data[7]);
            }

            else if (strstr(command_line[0], COMMAND_TIME) == 0) {
                short time[6];
                get_datetime(time);
                printf("\nGIORNO: %i/%i/%i\tTEMPO: %i:%i:%i", time[3], time[4], time[5], 
                                                                time[2], time[1], time[0]);
            }

        //====================
        //  DEFAULT SHELL COMMANDS CLEAR, ECHO AND HELP
        //====================
        //  FILE SYSTEM COMMANDS
        //====================

            else if (strstr(command_line[0], COMMAND_IN_DIR) == 0) {
                str2uppercase(command_line[1]);
                if (strstr(command_line[1], COMMAND_OUT_DIR) == 0) {
                    char* up_path = FSLIB_change_path(current_path, NULL);
                    if (up_path == NULL) {
                        up_path = malloc(5);
                        strcpy(up_path, "HOME");
                    }

                    free(current_path);
                    current_path = up_path;         
                }

                else {
                    char* dir_path = FSLIB_change_path(current_path, command_line[1]);
                    if (cexists(current_path) == 0) {
                        free(dir_path);
                        printf("\nLA DIRECTORY NON ESISTE");
                        return;
                    }

                    Content* content = get_content(dir_path);
                    if (content->file != NULL) {
                        FSLIB_unload_content_system(content);
                        free(dir_path);
                        printf("\nQUESTA NON E` UNA DIRECTORY");
                        return;
                    }

                    FSLIB_unload_content_system(content);
                    free(current_path);
                    current_path = dir_path;
                }
            }

            else if (strstr(command_line[0], COMMAND_LIST_DIR) == 0) {
                Directory* directory   = opendir(current_path);
                Directory* current_dir = directory->subDirectory;
                File* current_file     = directory->files;

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
                
                FSLIB_unload_directories_system(directory);
            }

            else if (strstr(command_line[0], COMMAND_FILE_VIEW) == 0) {
                char* file_path = FSLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    printf("\nLA FILE NON ESISTE");
                    free(file_path);
                    return;
                }
                
                printf("\n");

                Content* content = get_content(file_path);
                int data_size = 0;
                while (data_size < content->file->file_meta.file_size) {
                    int copy_size = min(content->file->file_meta.file_size - data_size, 128);
                    char* data = (char*)calloc(copy_size, 1);

                    fread_off(content, data_size, data, copy_size);
                    printf("%s", data);

                    free(data);
                    data_size += copy_size;
                }
                
                FSLIB_unload_content_system(content);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_BMP_SHOW) == 0) {
                char* file_path = FSLIB_change_path(current_path, command_line[1]);
                if (cexists(file_path) == 0) {
                    printf("\nLA FILE NON ESISTE");
                    free(file_path);
                    return;
                }
                
                bitmap_t* bitmap = BMP_create(file_path, atoi(command_line[2]), atoi(command_line[3]));
                
                BMP_display(bitmap);
                BMP_unload(bitmap);
                free(file_path);
            }

            else if (strstr(command_line[0], COMMAND_FILE_RUN) == 0) {
                char* exec_path = FSLIB_change_path(current_path, command_line[1]);
                if (cexists(exec_path) == 0) {
                    printf("\nLA FILE NON ESISTE");
                    free(exec_path);
                    return;
                }

                int pos = 2;
                char* exe_argv[COMMAND_BUFFER];
                while (command_line[pos] != NULL && pos < COMMAND_BUFFER) {
                    exe_argv[pos - 2] = command_line[pos];
                    pos++;
                }

                printf("\nCODE: [%i]\n", fexec(exec_path, pos - 2, exe_argv));
                free(exec_path);
            }

            else if (strstr(command_line[0], COMMAND_CINFO) == 0) {
                char* info_file = FSLIB_change_path(current_path, command_line[1]);
                if (cexists(info_file) == 0) {
                    printf("\nLA CONTENT NON ESISTE");
                    free(info_file);
                    return;
                }

                printf("\n");
                Content* content = get_content(info_file);
                if (content->directory != NULL) {
                    Directory* directory = content->directory;
                    Date* creation_date  = FSLIB_get_date(directory->directory_meta.creation_date, 1);
                    Date* accesed_date   = FSLIB_get_date(directory->directory_meta.last_modification_date, 1);

                    printf("DIRECTORY\n");
                    printf("NAME:          [%s]\n", directory->name);
                    printf("SIZE:          [%iB]\n", directory->directory_meta.file_size);
                    printf("CREATION DATE: [%i / %i / %i]\n", creation_date->day, creation_date->mounth, creation_date->year);
                    printf("ACCESED DATE:  [%i / %i / %i]\n", accesed_date->day, accesed_date->mounth, accesed_date->year);

                    free(creation_date);
                    free(accesed_date);
                }
                else {
                    File* file = content->file;
                    Date* creation_date  = FSLIB_get_date(file->file_meta.creation_date, 1);
                    Date* accesed_date   = FSLIB_get_date(file->file_meta.last_modification_date, 1);

                    printf("FILE\n");
                    printf("NAME:          [%s.%s]\n", file->name, file->extension);
                    printf("SIZE:          [%iB]\n", file->file_meta.file_size);
                    printf("CREATION DATE: [%i / %i / %i]\n", creation_date->day, creation_date->mounth, creation_date->year);
                    printf("ACCESED DATE:  [%i / %i / %i]\n", accesed_date->day, accesed_date->mounth, accesed_date->year);

                    free(creation_date);
                    free(accesed_date);
                }

                FSLIB_unload_content_system(content);
            }

        //====================
        //  FILE SYSTEM COMMANDS
        //====================
        //  NETWORKING COMMANDS
        //====================

            else if (strstr(command_line[0], COMMAND_IPCONFIG) == 0) {
                uint8_t ip[4]  = { 0x00 };
                uint8_t mac[6] = { 0xFF };

                get_ip(ip);
                get_mac(mac);

                printf("\nUTILITA` KERNEL IPCONF VERSIONE 0.2b\n");
                printf("\nIP ATTUALE:  [%i.%i.%i.%i]", ip[0], ip[1], ip[2], ip[3]);
                printf("\nMAC ATTUALE: [%x.%x.%x.%x.%x.%x]", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }

            else if (strstr(command_line[0], COMMAND_SEND_UDP_PACKET) == 0) {
                uint8_t ip[4] = { 0x00, 0x00, 0x00, 0x00 };
                get_ip(ip);

                uint8_t dst_ip[4] = { atoi(command_line[1]), atoi(command_line[2]), atoi(command_line[3]), atoi(command_line[4]) };
                uint16_t dst_port = atoi(command_line[5]);
                uint16_t src_port = atoi(command_line[6]);

                printf("\nTRASFERIMENTO [%s] DA [%i.%i.%i.%i:%i] A [%i.%i.%i.%i:%i]",
                                                command_line[7], ip[0], ip[1], ip[2], ip[3], src_port,
                                                dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3], dst_port);
                send_udp_packet(dst_ip, src_port, dst_port, command_line[7], strlen(command_line[7]));
            }

            else if (strstr(command_line[0], COMMAND_POP_UDP_PACKET) == 0) {
                uint8_t buffer[512];
                pop_received_udp_packet(buffer);

                printf("\n");
                printf("RICEVUTO PACCHETTO UDP STR:       [%s]\n", (char*)buffer);
                printf("RICEVUTO PACCHETTO UDP UINT:      [%u]\n", buffer);
                printf("RICEVUTO PACCHETTO UDP INT:       [%i]\n", buffer);
                printf("RICEVUTO PACCHETTO UDP HEX:       [%x]\n", buffer);
                printf("RICEVUTO PACCHETTO UDP POINTER:   [%p]\n", buffer);
            }

        //====================
        //  NETWORKING COMMANDS
        //====================

            else printf("\nCOMANDO [%s ...] SCONOSCUITO", command_line[0]);

        printf("\n");
    }

//====================
//  KSHELL COMMANDS
//====================