#include "manager.h"


int row_position = 1;
Content* current_directory;

int upper_border = 0;
int down_border = 14;

char* current_path;


void main(int argc, char* argv[]) {
    if (argc == 0) open_file_manager("HOME");
    else open_file_manager(argv[0]);
}

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

int set_line_color(int line, uint8_t color) {
    for (int i = 0; i < 80; i++) directly_putclr(i, (line + LINE_OFFSEET) - upper_border, color);
}

void open_file_manager(char* path) {
    char* current_path = malloc(strlen(path) + 1);
    strcpy(current_path, path);

    clrscr();
    print_directory_data();

    while (1) {
        char user_action = wait_char();
        if (user_action == UP_ARROW_BUTTON && row_position > 0) {
            if (abs(row_position - upper_border) <= 1 && upper_border >= 1) {
                upper_border--;
                down_border--;

                clrscr();
                print_directory_data();
            }

            set_line_color(row_position--, BACKGROUND_BLACK + FOREGROUND_WHITE);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE);
        }

        else if (user_action == DOWN_ARROW_BUTTON && row_position < down_border) {
            if (abs(row_position - down_border) <= 1) {
                upper_border++;
                down_border++;

                clrscr();
                print_directory_data();
            }

            set_line_color(row_position++, BACKGROUND_BLACK + FOREGROUND_WHITE);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE);
        }

        else if (user_action == ENTER_BUTTON || user_action == BACKSPACE_BUTTON) {
            execute_item(user_action);
            clrscr();
            print_directory_data();
        }

        else if (user_action == F3_BUTTON) {
            row_position = 1;
            clrscr();
            break;
        }

        else if (user_action == F1_BUTTON) {
            cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nDIR NAME: ");

            char dir_name[40] = { '\n' };
            input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, dir_name);
            mkdir(current_path, dir_name);
            free(dir_name); 

            clrscr();
            print_directory_data();
        }

        else if (user_action == F2_BUTTON) {
            cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nFILE NAME: ");

            char file_name[40] = { '\n' };
            input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, file_name);
            mkfile(current_path, file_name);
            free(file_name);

            clrscr();
            print_directory_data();
        }

        else if (user_action == F4_BUTTON) {
            execute_item(user_action);
            clrscr();
            print_directory_data();
        }
    }
}

void execute_item(char action_type) {
    if (row_position == 0 && action_type != BACKSPACE_BUTTON) {
        current_path = FSLIB_change_path(current_path, NULL);
        if (strlen(current_path) <= 1) {
            free(current_path);
            current_path = malloc(4);
            strcpy(current_path, "BOOT");
        }

        return;
    }
    
    //=====================
    //  SELECTED DIRECTORY
    //=====================

        int rows = 1;
        Directory* currentDir = opendir(current_path);
        Directory* subdir     = currentDir->subDirectory;
        while (subdir != NULL) {
            if (rows++ == row_position) {
                if (action_type == ENTER_BUTTON) 
                    current_path = FSLIB_change_path(current_path, subdir->name);

                else if (action_type == BACKSPACE_BUTTON) {
                    printf("\nDELETE? (Y/N): ");
                    while (1) {
                        char answer[10] = { '\n' };
                        input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, answer);
                        if (strcmp(answer, "y") == 0) {
                            rmcontent(current_path, subdir->name);
                            free(answer);
                            break;
                        }
                        
                        free(answer);
                        break;
                    }
                }

                else if (action_type == F4_BUTTON) { 
                    cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW DIR NAME: ");
                    char new_dir_name[40] = { '\n' };
                    input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, new_dir_name);

                    directory_entry_t* new_meta = FSLIB_create_entry(new_dir_name, NULL, 1, NULL, NULL);
                    current_path = (FSLIB_change_path(current_path, subdir->directory_meta.file_name));
                    chgcontent(current_path, new_meta);
                    current_path = (FSLIB_change_path(current_path, NULL));

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

        File* currentFile = currentDir->files;
        while (currentFile != NULL) {

            char name[25]; 
            FAT_fatname2name(currentFile->file_meta.file_name, name);

            if (rows++ == row_position) {
                row_position = 1;

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
                                if (row_position == i) cprintf(BACKGROUND_RED + FOREGROUND_WHITE, lines[i]);
                                else printf(lines[i]);

                                printf(LINE);
                            }

                            char user_action = wait_char();
                            if (user_action == UP_ARROW_BUTTON && row_position > 0) row_position--;
                            else if (user_action == DOWN_ARROW_BUTTON && row_position < 4) row_position++;
                            else if (user_action == ENTER_BUTTON) {
                                switch (row_position) {
                                    case EDIT_POS:
                                        current_path = (FSLIB_change_path(current_path, name));
                                        text_editor_init(current_path, BACKGROUND_BLACK + FOREGROUND_WHITE);
                                        current_path = (FSLIB_change_path(current_path, NULL));

                                    break;

                                    case VIEW_POS:
                                        clrscr();

                                        current_path = (FSLIB_change_path(current_path, name));
                                        char* data = fread(current_path);
                                        printf("%sFILE: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                                        current_path = (FSLIB_change_path(current_path, NULL));

                                        set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                                        free(data);
                                        keyboard_wait(F3_BUTTON);
                                    break;

                                    case RENAME_POS:
                                        cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE NAME: ");
                                        char new_file_name[40] = { '\n' };
                                        input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, new_file_name);

                                        cprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE EXT: ");
                                        char new_file_ext[40] = { '\n' };
                                        input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, new_file_ext);

                                        directory_entry_t* new_meta = FSLIB_create_entry(new_file_name, new_file_ext, 0, NULL, NULL);

                                        current_path = (FSLIB_change_path(current_path, name));
                                        chgcontent(current_path, new_meta);
                                        current_path = (FSLIB_change_path(current_path, NULL));
                                        
                                        free(new_file_name);
                                        free(new_file_ext);
                                        free(new_meta);

                                    break;

                                    case DELETE_POS:
                                        printf("\nDELETE? (Y/N): ");
                                        
                                        char user_choose[40] = { '\n' };
                                        input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, user_choose);
                                        if (strcmp(user_choose, "y") == 0) {
                                            rmcontent(current_path, currentFile->name);
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

                            current_path = (FSLIB_change_path(current_path, name));
                            char* data = fread(current_path);
                            printf("%sFile: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                            current_path = (FSLIB_change_path(current_path, NULL));
                
                            free(data);

                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        if (strstr(currentFile->extension, "ELF") == 0) {
                            clrscr();
                            
                            current_path = (FSLIB_change_path(current_path, name));
                            printf("\nEXIT CODE: [%i]", fexec(current_path, NULL, NULL));
                            current_path = (FSLIB_change_path(current_path, NULL));

                            printf("\n\nPress [F3] to exit");
                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        else if (strstr(currentFile->extension, "ASM") == 0) {
                            clrscr();
                            current_path = (FSLIB_change_path(current_path, name));
                            char* data = fread(current_path);
                            asm_execute(data);
                            current_path = (FSLIB_change_path(current_path, NULL));

                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                            free(data);
                            printf("\n\nPress [F3] to exit");
                            keyboard_wait(F3_BUTTON);

                            break;
                        }

                        else if (strstr(currentFile->extension, "SHL") == 0) {
                            clrscr();
                            set_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                            
                            current_path = (FSLIB_change_path(current_path, name));
                            char* sector_data = fread(current_path);
                            current_path = (FSLIB_change_path(current_path, NULL));

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
                        
                        char user_choose[10] = { '\n' };
                        input_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE, user_choose);
                        if (strcmp(user_choose, "y") == 0) {
                            rmcontent(current_path, name);
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

        FSLIB_unload_files_system(currentFile);

    //=========================
    //  SELECTED DIRECTORY
    //=========================

}

void print_directory_data() {
    printf("Directory: [%s]\n%s%s%s", current_path, LINE, HEADER, LINE);

    int rows = upper_border;
    int all_rows = 0;
    if (all_rows++ == rows) {
        printf(UPPED_DIR);
        rows++;
    }

    Directory* currentDir = opendir(current_path);
    Directory* subdirs    = currentDir->subDirectory;
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

    File* currentFile = currentDir->files;
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

            Date* creation_date     = FSLIB_get_date(currentFile->file_meta.creation_date, 1);
            Date* modification_date = FSLIB_get_date(currentFile->file_meta.last_modification_date, 1);
            Date* access_date       = FSLIB_get_date(currentFile->file_meta.last_accessed, 1);

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

    FSLIB_unload_directories_system(currentDir);

    for (int i = 0; i < 15 - (rows - upper_border); i++) printf(EMPTY);
    printf("%s[F1 - MKDIR] [F2 - MKFILE] [F3 - EXIT] [F4 - EDIT] [ENTER - EXEC] [BSPACE - RM]\n", LINE);
}
