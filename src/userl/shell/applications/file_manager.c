#include <stdint.h>

#include "../include/file_manager.h"


int row_position = 1;
struct FATContent* current_directory;

int upper_border = 0;
int down_border = 14;

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
    for (int i = 0; i < 80; i++)
        VGA_putcolor(i, (line + LINE_OFFSEET) - upper_border, color);
}

void open_file_manager(struct User* user) {
    current_directory = FAT_get_content(FAT_get_current_path());

    VGA_clrscr();
    print_directory_data();

    while(1) {
        char user_action = keyboard_navigation();
        if (user_action == UP_ARROW_BUTTON && row_position > 0) {
            if (abs(row_position - upper_border) <= 1 && upper_border >= 1) {
                upper_border--;
                down_border--;

                VGA_clrscr();
                print_directory_data();
            }

            set_line_color(row_position--, BACKGROUND_BLACK + FOREGROUND_WHITE);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE);
        }

        else if (user_action == DOWN_ARROW_BUTTON && row_position < down_border) {
            if (abs(row_position - down_border) <= 1) {
                upper_border++;
                down_border++;

                VGA_clrscr();
                print_directory_data();
            }

            set_line_color(row_position++, BACKGROUND_BLACK + FOREGROUND_WHITE);
            set_line_color(row_position, BACKGROUND_RED + FOREGROUND_WHITE);
        }

        else if (user_action == ENTER_BUTTON || user_action == BACKSPACE_BUTTON) {
            execute_item(user, user_action);
            VGA_clrscr();
            print_directory_data();
        }

        else if (user_action == F3_BUTTON) {
            row_position = 1;
            VGA_clrscr();
            break;
        }

        else if (user_action == F1_BUTTON) {
            kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nDIR NAME: ");
            char* dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
            
            struct FATContent* content = FAT_create_content(dir_name, 1, NULL);
            FAT_put_content(FAT_get_current_path(), content);
            
            free(dir_name); 
            FAT_unload_content_system(content);

            FAT_unload_content_system(current_directory);
            current_directory = FAT_get_content(FAT_get_current_path());

            VGA_clrscr();
            print_directory_data();
        }

        else if (user_action == F2_BUTTON) {
            kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nFILE NAME: ");
            char* file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
            
            kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\t\tFILE EXT: ");
            char* file_extension = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

            struct FATContent* content = FAT_create_content(file_name, 0, file_extension); 
            FAT_put_content(FAT_get_current_path(), content);

            free(file_name);
            free(file_extension);
            FAT_unload_content_system(content);

            FAT_unload_content_system(current_directory);
            current_directory = FAT_get_content(FAT_get_current_path());

            VGA_clrscr();
            print_directory_data();
        }

        else if (user_action == F4_BUTTON) {
            execute_item(user, user_action);
            VGA_clrscr();
            print_directory_data();
        }
    }
}

void execute_item(struct User* user, char action_type) {
    if (row_position == 0 && action_type != BACKSPACE_BUTTON) {
        FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
        if (strlen(FAT_get_current_path()) <= 1) 
            FAT_set_current_path("BOOT");

        FAT_unload_content_system(current_directory);
        current_directory = FAT_get_content(FAT_get_current_path());

        return;
    }
    
    ///////////////////////////////////
    //  SELECTED DIRECTORY
    //

        int rows = 1;
        struct FATContent* content = FAT_get_content(FAT_get_current_path());
        if (content->directory != NULL) {
            struct UFATDirectory* currentDir = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, 0)->subDirectory;
            while (currentDir != NULL) {
                if (rows++ == row_position) {
                    if (action_type == ENTER_BUTTON) {
                        FAT_set_current_path(FAT_change_path(FAT_get_current_path(), currentDir->name));
                        FAT_unload_content_system(current_directory);
                        current_directory = FAT_get_content(FAT_get_current_path());
                    }

                    else if (action_type == BACKSPACE_BUTTON) {
                        kprintf("\nDELETE? (Y/N): ");
                        while (1) {
                            char* answer = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                            if (strcmp(answer, "y") == 0) {
                                FAT_delete_content(FAT_get_current_path(), currentDir->name);
                                free(answer);

                                FAT_unload_content_system(current_directory);
                                current_directory = FAT_get_content(FAT_get_current_path());

                                break;
                            }
                            
                            free(answer);
                            break;
                        }
                    }

                    else if (action_type == F4_BUTTON) { 
                        kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW DIR NAME: ");
                        char* new_dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                        udirectory_entry_t* new_meta = FAT_create_entry(new_dir_name, NULL, 1, NULL, NULL);
                        FAT_set_current_path(FAT_change_path(FAT_get_current_path(), currentDir->directory_meta.file_name));
                        FAT_change_meta(FAT_get_current_path(), new_meta);
                        FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                        free(new_dir_name);
                        free(new_meta);

                        FAT_unload_content_system(current_directory);
                        current_directory = FAT_get_content(FAT_get_current_path());
                    }

                    break;
                }

                currentDir = currentDir->next;
            }

            FAT_unload_directories_system(currentDir);
        }

    //
    //  SELECTED DIRECTORY
    ///////////////////////////////////

    ///////////////////////////////////
    //  SELECTED FILE
    //

        if (content->directory != NULL) {
            struct FATFile* currentFile = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, 0)->files;
            while (currentFile != NULL) {

                char name[25]; 
                FAT_fatname2name(currentFile->file_meta.file_name, name);

                if (rows++ == row_position) {
                    row_position = 1;

                    /////////////////
                    //  EDIT
                    //

                        if (action_type == F4_BUTTON) { 
                            while(1) {
                                kset_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                            
                                kprintf("\n%s", LINE);
                                const char* lines[4] = {
                                    EDIT_LINE,
                                    VIEW_LINE,
                                    RENAME_LINE,
                                    DELETE_LINE
                                };

                                for (int i = EDIT_POS; i <= DELETE_POS; i++) {
                                    if (row_position == i) kcprintf(BACKGROUND_RED + FOREGROUND_WHITE, lines[i]);
                                    else kprintf(lines[i]);

                                    kprintf(LINE);
                                }

                                char user_action = keyboard_navigation();
                                if (user_action == UP_ARROW_BUTTON && row_position > 0) 
                                    row_position--;

                                else if (user_action == DOWN_ARROW_BUTTON && row_position < 4)
                                    row_position++;

                                else if (user_action == ENTER_BUTTON) {
                                    switch (row_position) {
                                        case EDIT_POS:
                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                            text_editor_init(FAT_get_current_path(), BACKGROUND_BLACK + FOREGROUND_WHITE);
                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                        break;

                                        case VIEW_POS:
                                            VGA_clrscr();

                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                            char* data = FAT_read_content(FAT_get_content(FAT_get_current_path()));
                                            kprintf("%sFILE: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                            kset_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                                            free(data);
                                            keyboard_wait(F3_BUTTON);
                                        break;

                                        case RENAME_POS:
                                            kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE NAME: ");
                                            char* new_file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                                            kcprintf(BACKGROUND_BLACK + FOREGROUND_WHITE, "\nNEW FILE EXT: ");
                                            char* new_file_ext = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);

                                            udirectory_entry_t* new_meta = FAT_create_entry(new_file_name, new_file_ext, 0, NULL, NULL);

                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                            FAT_change_meta(FAT_get_current_path(), new_file_name);
                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                                            
                                            free(new_file_name);
                                            free(new_file_ext);
                                            free(new_meta);

                                            FAT_unload_content_system(current_directory);
                                            current_directory = FAT_get_content(FAT_get_current_path());

                                        break;

                                        case DELETE_POS:
                                            kprintf("\nDELETE? (Y/N): ");
                                            
                                            char* user_choose = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                                            if (strcmp(user_choose, "y") == 0) {
                                                FAT_delete_content(FAT_get_current_path(), currentFile->name);
                                                
                                                FAT_unload_content_system(current_directory);
                                                current_directory = FAT_get_content(FAT_get_current_path());
                                            }

                                            VGA_clrscr();

                                            free(user_choose);

                                        break;
                                    }

                                    return;
                                }
                                
                                else if (user_action == F3_BUTTON) {
                                    VGA_clrscr();
                                    break;
                                }

                                VGA_clrscr();
                            }
                        }

                    //
                    //  EDIT
                    /////////////////
                    //  EXECUTE
                    //
                        else if (action_type == ENTER_BUTTON) {
                            if (strstr(currentFile->extension, "TXT") == 0) {
                                VGA_clrscr();

                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                char* data = FAT_read_content(FAT_get_content(FAT_get_current_path()));
                                kprintf("%sFile: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, data, LINE);
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));
                                free(data);

                                kset_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                                keyboard_wait(F3_BUTTON);

                                break;
                            }

                            if (strstr(currentFile->extension, "ELF") == 0) {
                                VGA_clrscr();
                                
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                kprintf("\nEXIT CODE: [%i]", FAT_ELF_execute_content(FAT_get_current_path(), NULL, NULL));
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                kprintf("\n\nPress [F3] to exit");
                                keyboard_wait(F3_BUTTON);

                                break;
                            }

                            else if (strstr(currentFile->extension, "ASM") == 0) {
                                VGA_clrscr();
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                char* data = FAT_read_content(FAT_get_content(FAT_get_current_path()));
                                asm_execute(data);
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                kset_color(BACKGROUND_BLACK + FOREGROUND_WHITE);

                                free(data);
                                kprintf("\n\nPress [F3] to exit");
                                keyboard_wait(F3_BUTTON);

                                break;
                            }

                            else if (strstr(currentFile->extension, "SHL") == 0) {
                                VGA_clrscr();
                                kset_color(BACKGROUND_BLACK + FOREGROUND_WHITE);
                                
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                char* sector_data = FAT_get_content(FAT_get_current_path())->file->data;
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

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
                                
                                kprintf("\n\nPress [F3] to exit");
                                keyboard_wait(F3_BUTTON);
                                
                                break;
                            }
                        }

                        else if (action_type == BACKSPACE_BUTTON) {
                            kprintf("\nDelete? (Y/N): ");
                                            
                            char* user_choose = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLACK + FOREGROUND_WHITE);
                            if (strcmp(user_choose, "y") == 0) {
                                FAT_delete_content(FAT_get_current_path(), name);
                                
                                FAT_unload_content_system(current_directory);
                                current_directory = FAT_get_content(FAT_get_current_path());
                            }

                            free(user_choose);
                        }

                    //
                    //  EXECUTE
                    /////////////////

                    break;
                }

                currentFile = currentFile->next;
            }

            FAT_unload_files_system(currentFile);
        }

        FAT_unload_content_system(content);

    //
    //  SELECTED DIRECTORY
    ///////////////////////////////////

}

void print_directory_data() {
    kprintf("Directory: [%s]\n%s%s%s", FAT_get_current_path(), LINE, HEADER, LINE);

    int rows = upper_border;
    int all_rows = 0;
    if (all_rows++ == rows) {
        kprintf(UPPED_DIR);
        rows++;
    }

    struct UFATDirectory* currentDir = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(current_directory->directory->directory_meta), NULL, 0)->subDirectory;
    while (currentDir != NULL) {
        char name[12];
        name[11] = '\0';

        strncpy(name, currentDir->name, 11);
        for (size_t i = strlen(name); i < 11; i++) name[i] = ' ';

        if (all_rows++ == rows) {
            kprintf("| %s | DIR    | N/A      | N/A      | N/A       | N/A  | N/A         |\n", name);
            rows++;
        }

        currentDir = currentDir->next;
    }

    struct FATFile* currentFile = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(current_directory->directory->directory_meta), NULL, 0)->files;
    while (currentFile != NULL) {
        //////////////////////////
        //  FILE INFO PREPARATIONS

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

            struct FATDate* creation_date = FAT_get_date(currentFile->file_meta.creation_date, 1);
            struct FATDate* modification_date = FAT_get_date(currentFile->file_meta.last_modification_date, 1);
            struct FATDate* access_date = FAT_get_date(currentFile->file_meta.last_accessed, 1);

            for (size_t i = strlen(file_name); i < 11; i++) file_name[i] = ' ';
            for (size_t i = strlen(file_extension); i < 4; i++) file_extension[i] = ' ';
            for (size_t i = strlen(file_size); i < 11; i++) file_size[i] = ' ';

        //  FILE INFO PREPARATIONS
        //////////////////////////

        char* cdate = convert_date(creation_date->day, creation_date->mounth, creation_date->year);
        char* mdate = convert_date(modification_date->day, modification_date->mounth, modification_date->year);
        char* adate = convert_date(access_date->day, access_date->mounth, access_date->year);

        if (all_rows++ == rows) {
            kprintf("| %s | FILE   | %s | %s | %s  | %s | %s |\n", file_name, cdate, mdate, adate, file_extension, file_size);
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

    for (int i = 0; i < 15 - (rows - upper_border); i++) kprintf(EMPTY);
    kprintf("%s[F1 - MKDIR] [F2 - MKFILE] [F3 - EXIT] [F4 - EDIT] [ENTER - EXEC] [BSPACE - RM]\n", LINE);
}
