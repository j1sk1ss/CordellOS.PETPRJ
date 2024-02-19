#include <stdint.h>

#include "file_manager.h"

void keyboard_wait(char symbol) {
    while (1) {
        char user_action = wait_char();
        if (user_action == symbol) break;
    }
}

char* convert_date(int day, int month, int year) {
    char* date = (char*)malloc(9);

    date[0] = '0' + (day / 10);
    date[1] = '0' + (day % 10);
    date[2] = '.';
    date[3] = '0' + (month / 10);
    date[4] = '0' + (month % 10);
    date[5] = '.';
    date[6] = '0' + ((year % 100) / 10);
    date[7] = '0' + ((year % 100) % 10);
    date[8] = '\0';

    return date;
}

int set_line_color(int line, uint8_t color, int upper) {
    for (int i = 0; i < 80; i++) 
        directly_putclr(i, (line + LINE_OFFSEET) - upper, color);
}

void open_file_manager(char* path) {
    char* file_manager_path = malloc(strlen(path));
    strcpy(file_manager_path, path);

    int row_position = 1;
    int upper_border = 0;
    int down_border  = 14;

    clrscr();
    print_directory_data(file_manager_path, upper_border);

    while(1) {
        char user_action = wait_char();
        if (user_action == UP_ARROW_BUTTON && row_position > 0) {
            if (abs(row_position - upper_border) <= 1 && upper_border >= 1) {
                upper_border--;
                down_border--;

                clrscr();
                print_directory_data(file_manager_path, upper_border);
            }

            set_line_color(row_position--, BACKGROUND_BLACK + FOREGROUND_WHITE, upper_border);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE, upper_border);
        }

        else if (user_action == DOWN_ARROW_BUTTON && row_position < down_border) {
            if (abs(row_position - down_border) <= 1) {
                upper_border++;
                down_border++;

                clrscr();
                print_directory_data(file_manager_path, upper_border);
            }

            set_line_color(row_position++, BACKGROUND_BLACK + FOREGROUND_WHITE, upper_border);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE, upper_border);
        }

        else if (user_action == ENTER_BUTTON || user_action == BACKSPACE_BUTTON) {
            execute_item(user_action, &file_manager_path, &row_position);
            clrscr();
            print_directory_data(file_manager_path, upper_border);
        }

        else if (user_action == F3_BUTTON) {
            row_position = 1;
            clrscr();
            break;
        }

        else if (user_action == F1_BUTTON) {
            cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nDIR NAME: ");
            char* dir_name = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
            mkdir(file_manager_path, dir_name);
            free(dir_name); 

            clrscr();
            print_directory_data(file_manager_path, upper_border);
        }

        else if (user_action == F2_BUTTON) {
            cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nFILE NAME: ");
            char* file_name = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
            mkfile(file_manager_path, file_name);
            free(file_name);

            clrscr();
            print_directory_data(file_manager_path, upper_border);
        }

        else if (user_action == F4_BUTTON) {
            execute_item(user_action, &file_manager_path, &row_position);
            clrscr();
            print_directory_data(file_manager_path, upper_border);
        }
    }
}

void execute_item(char action_type, char** path, int* row) {
    if (*row == 0 && action_type != BACKSPACE_BUTTON) {
        *path = FATLIB_change_path(*path, NULL);
        if (strlen(*path) <= 1) {
            free(*path);
            *path = malloc(4);
            strcpy(*path, "BOOT");
        }

        return;
    }
    
    //=====================
    //  SELECTED DIRECTORY
    //=====================

        int rows = 1;
        struct UFATDirectory* currentDir = opendir(*path);
        struct UFATDirectory* subdir     = currentDir->subDirectory;
        while (subdir != NULL) {
            if (rows++ == *row) {
                if (action_type == ENTER_BUTTON) 
                    *path = FATLIB_change_path(*path, subdir->name);

                else if (action_type == BACKSPACE_BUTTON) {
                    printf("\nDELETE? (Y/N): ");
                    while (1) {
                        char* answer = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                        if (strcmp(answer, "y") == 0) {
                            rmcontent(*path, subdir->name);
                            free(answer);
                            break;
                        }
                        
                        free(answer);
                        break;
                    }
                }

                else if (action_type == F4_BUTTON) { 
                    cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW DIR NAME: ");
                    char* new_dir_name = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                    udirectory_entry_t* new_meta = FATLIB_create_entry(new_dir_name, NULL, 1, NULL, NULL);
                    *path = (FATLIB_change_path(*path, subdir->directory_meta.file_name));
                    chgcontent(*path, new_meta);
                    *path = (FATLIB_change_path(*path, NULL));

                    free(new_dir_name);
                    free(new_meta);
                }

                break;
            }

            subdir = subdir->next;
        }

    //=========================
    //  SELECTED DIRECTORY
    //=========================
    //  SELECTED FILE
    //=========================

        struct UFATFile* currentFile = currentDir->files;
        while (currentFile != NULL) {

            char name[25]; 
            FATLIB_fatname2name(currentFile->file_meta.file_name, name);

            if (rows++ == *row) {
                *row = 1;

                //=========================
                //  EDIT
                //=========================

                    if (action_type == F4_BUTTON) { 
                        while(1) {
                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                        
                            printf("\n%s", LINE);
                            const char* lines[4] = {
                                EDIT_LINE,
                                VIEW_LINE,
                                RENAME_LINE,
                                DELETE_LINE
                            };

                            for (int i = EDIT_POS; i <= DELETE_POS; i++) {
                                if (*row == i) cprintf(BACKGROUND_RED + FOREGROUND_WHITE, lines[i]);
                                else printf(lines[i]);

                                printf(LINE);
                            }

                            char user_action = wait_char();
                            if (user_action == UP_ARROW_BUTTON && *row > 0) *row--;
                            else if (user_action == DOWN_ARROW_BUTTON && *row < 4) *row++;
                            else if (user_action == ENTER_BUTTON) {
                                switch (*row) {
                                    case EDIT_POS:
                                        *path = (FATLIB_change_path(*path, name));
                                        text_editor_init(*path, BACKGROUND_BLACK + FOREGROUND_WHITE);
                                        *path = (FATLIB_change_path(*path, NULL));

                                    break;

                                    case VIEW_POS:
                                        clrscr();

                                        *path = (FATLIB_change_path(*path, name));
                                        char* data = fread(*path);
                                        printf("%sFILE: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                                        *path = (FATLIB_change_path(*path, NULL));

                                        set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                                        free(data);
                                        keyboard_wait(F3_BUTTON);
                                    break;

                                    case RENAME_POS:
                                        cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE NAME: ");
                                        char* new_file_name = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                                        cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE EXT: ");
                                        char* new_file_ext = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                                        udirectory_entry_t* new_meta = FATLIB_create_entry(new_file_name, new_file_ext, 0, NULL, NULL);

                                        *path = (FATLIB_change_path(*path, name));
                                        chgcontent(*path, new_meta);
                                        *path = (FATLIB_change_path(*path, NULL));
                                        
                                        free(new_file_name);
                                        free(new_file_ext);
                                        free(new_meta);

                                    break;

                                    case DELETE_POS:
                                        printf("\nDELETE? (Y/N): ");
                                        
                                        char* user_choose = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                                        if (strcmp(user_choose, "y") == 0) {
                                            rmcontent(*path, currentFile->name);
                                        }

                                        clrscr();
                                        free(user_choose);

                                    break;
                                }

                                return;
                            }
                            
                            else if (user_action == F3_BUTTON) {
                                clrscr();
                                break;
                            }

                            clrscr();
                        }
                    }

                //=========================
                //  EDIT
                //=========================
                //  EXECUTE
                //=========================

                    else if (action_type == ENTER_BUTTON) {
                        if (strstr(currentFile->extension, "TXT") == 0) {
                            clrscr();

                            *path = (FATLIB_change_path(*path, name));
                            char* data = fread(*path);
                            printf("%sFile: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                            *path = (FATLIB_change_path(*path, NULL));
                
                            free(data);

                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        if (strstr(currentFile->extension, "ELF") == 0) {
                            clrscr();
                            
                            *path = (FATLIB_change_path(*path, name));
                            printf("\nEXIT CODE: [%i]", fexec(*path, NULL, NULL));
                            *path = (FATLIB_change_path(*path, NULL));

                            printf("\n\nPress [F3] to exit");
                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        else if (strstr(currentFile->extension, "ASM") == 0) {
                            clrscr();
                            *path = (FATLIB_change_path(*path, name));
                            char* data = fread(*path);
                            asm_execute(data);
                            *path = (FATLIB_change_path(*path, NULL));

                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                            free(data);
                            printf("\n\nPress [F3] to exit");
                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        else if (strstr(currentFile->extension, "SHL") == 0) {
                            clrscr();
                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                            
                            *path = (FATLIB_change_path(*path, name));
                            char* sector_data = fread(*path);
                            *path = (FATLIB_change_path(*path, NULL));

                            char* command_for_split = (char*)malloc(strlen(sector_data));
                            strcpy(command_for_split, sector_data);

                            char* lines[100];
                            int tokenCount = 0;

                            char* splitted = strtok(command_for_split, "\n");
                            while(splitted) {
                                lines[tokenCount++] = splitted;
                                splitted = strtok(NULL, "\n");
                            }

                            for (int i = 0; i < tokenCount; i++) {
                                execute_command(lines[i], 2);
                                free(lines[i]);
                            }

                            free(command_for_split);
                            free(sector_data);
                            
                            printf("\n\nPress [F3] to exit");
                            keyboard_wait(F3_BUTTON);
                            
                            break;
                        }
                    }

                    else if (action_type == BACKSPACE_BUTTON) {
                        printf("\nDelete? (Y/N): ");
                                        
                        char* user_choose = input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                        if (strcmp(user_choose, "y") == 0) {
                            rmcontent(*path, name);
                        }

                        free(user_choose);
                    }

                //=========================
                //  EXECUTE
                //=========================

                break;
            }

            currentFile = currentFile->next;
        }

        FATLIB_unload_files_system(currentFile);

    //=========================
    //  SELECTED DIRECTORY
    //=========================

}

void print_directory_data(char* path, int upper) {
    printf("Directory: [%s]\n%s%s%s", path, LINE, HEADER, LINE);

    int rows = upper;
    int all_rows = 0;
    if (all_rows++ == rows) {
        printf(UPPED_DIR);
        rows++;
    }

    struct UFATDirectory* currentDir = opendir(path);
    struct UFATDirectory* subdirs    = currentDir->subDirectory;
    while (subdirs != NULL) {
        char name[12];
        name[11] = '\0';

        strncpy(name, subdirs->name, 11);
        for (size_t i = strlen(name); i < 11; i++) name[i] = ' ';

        if (all_rows++ == rows) {
            printf("| %s | DIR    | N/A      | N/A      | N/A       | N/A  | N/A         |\n", name);
            rows++;
        }

        subdirs = subdirs->next;
    }

    struct UFATFile* currentFile = currentDir->files;
    while (currentFile != NULL) {

        //=========================
        //  FILE INFO PREPARATIONS
        //=========================

            char file_name[12];
            char file_extension[5];
            char file_size[12];

            file_name[11] = '\0';
            file_extension[4] = '\0';
            file_size[11] = '\0';

            strncpy(file_name, currentFile->name, 11);
            strncpy(file_extension, currentFile->extension, 4);
            
            char* file_size_str = itoa(currentFile->file_meta.file_size);
            strncpy(file_size, file_size_str, 10);
            strcat(file_size, "B");

            struct UFATDate* creation_date     = FATLIB_get_date(currentFile->file_meta.creation_date, 1);
            struct UFATDate* modification_date = FATLIB_get_date(currentFile->file_meta.last_modification_date, 1);
            struct UFATDate* access_date       = FATLIB_get_date(currentFile->file_meta.last_accessed, 1);

            for (size_t i = strlen(file_name); i < 11; i++) file_name[i] = ' ';
            for (size_t i = strlen(file_extension); i < 4; i++) file_extension[i] = ' ';
            for (size_t i = strlen(file_size); i < 11; i++) file_size[i] = ' ';

        //=========================
        //  FILE INFO PREPARATIONS
        //=========================

        char* cdate = convert_date(creation_date->day, creation_date->mounth, creation_date->year);
        char* mdate = convert_date(modification_date->day, modification_date->mounth, modification_date->year);
        char* adate = convert_date(access_date->day, access_date->mounth, access_date->year);

        if (all_rows++ == rows) {
            printf("| %s | FILE   | %s | %s | %s  | %s | %s |\n", file_name, cdate, mdate, adate, file_extension, file_size);
            rows++;
        }

        currentFile = currentFile->next;

        free(creation_date);
        free(modification_date);
        free(access_date);
        free(file_size_str);

        free(cdate);
        free(mdate);
        free(adate);
    }

    FATLIB_unload_directories_system(currentDir);

    for (int i = 0; i < 15 - (rows - upper); i++) printf(EMPTY);
    printf("%s[F1 - MKDIR] [F2 - MKFILE] [F3 - EXIT] [F4 - EDIT] [ENTER - EXEC] [BSPACE - RM]\n", LINE);
}
