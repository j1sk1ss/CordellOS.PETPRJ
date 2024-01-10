#include <stdint.h>

#include "../include/file_manager.h"


int row_position = 1;
struct FATContent* current_directory;

void open_file_manager(struct User* user) {
    current_directory = FAT_get_content(FAT_get_current_path());

    VGA_clrscr();
    print_directory_data();

    while(1) {
        char user_action = keyboard_navigation();
        if (user_action == UP_ARROW_BUTTON && row_position > 0) row_position--;
        else if (user_action == DOWN_ARROW_BUTTON) row_position++;
        else if (user_action == ENTER_BUTTON || user_action == BACKSPACE_BUTTON) execute_item(user, user_action);

        else if (user_action == F3_BUTTON) {
            VGA_clrscr();
            break;
        }

        else if (user_action == F1_BUTTON) {
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nDir name: ");
            char* dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
            
            struct FATContent* content = FAT_create_content(dir_name, TRUE, NULL);
            FAT_put_content(FAT_get_current_path(), content);
            
            free(dir_name); 
            FAT_unload_content_system(content);

            FAT_unload_content_system(current_directory);
            current_directory = FAT_get_content(FAT_get_current_path());
        }

        else if (user_action == F2_BUTTON) {
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nFile name: ");
            char* file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
            
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\t\tFile extension: ");
            char* file_extension = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

            struct FATContent* content = FAT_create_content(file_name, FALSE, file_extension); 
            FAT_put_content(FAT_get_current_path(), content);

            free(file_name);
            free(file_extension);
            FAT_unload_content_system(content);

            FAT_unload_content_system(current_directory);
            current_directory = FAT_get_content(FAT_get_current_path());
        }

        else if (user_action == F4_BUTTON) execute_item(user, user_action);
        
        VGA_text_clrscr();
        print_directory_data();
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
            struct FATDirectory* currentDir = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE)->subDirectory;
            while (currentDir != NULL) {
                if (rows++ == row_position) {
                    if (action_type == ENTER_BUTTON) {
                        FAT_set_current_path(FAT_change_path(FAT_get_current_path(), currentDir->name));
                        FAT_unload_content_system(current_directory);
                        current_directory = FAT_get_content(FAT_get_current_path());
                    }

                    else if (action_type == BACKSPACE_BUTTON) {
                        printf("\nDelete? (Y/N): ");
                        while (1) {
                            char* answer = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
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
                        cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew dir name: ");
                        char* new_dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                        directory_entry_t* new_meta = FAT_create_entry(new_dir_name, NULL, TRUE, NULL, NULL);
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
            struct FATFile* currentFile = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE)->files;
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
                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                            
                                printf("\n%s", LINE);
                                const char* lines[3] = {
                                    EDIT_LINE,
                                    RENAME_LINE,
                                    DELETE_LINE
                                };

                                for (int i = EDIT_POS; i <= DELETE_POS; i++) {
                                    if (row_position == i) cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, lines[i]);
                                    else printf(lines[i]);

                                    printf(LINE);
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
                                            text_editor_init(FAT_get_current_path(), BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                            FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                        break;

                                        case RENAME_POS:
                                            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew file name: ");
                                            char* new_file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                                            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew file ext: ");
                                            char* new_file_ext = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                                            directory_entry_t* new_meta = FAT_create_entry(new_file_name, new_file_ext, FALSE, NULL, NULL);

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
                                            printf("\nDelete? (Y/N): ");
                                            
                                            char* user_choose = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
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
                                printf("%sFile: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, FAT_get_content(FAT_get_current_path())->file->data, LINE);
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                                keyboard_wait(F3_BUTTON);

                                break;
                            }

                            else if (strstr(currentFile->extension, "ASM") == 0) {
                                VGA_clrscr();
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), name));
                                asm_execute(FAT_get_content(FAT_get_current_path())->file->data);
                                FAT_set_current_path(FAT_change_path(FAT_get_current_path(), NULL));

                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                                printf("\n\nPress [F3] to exit");
                                keyboard_wait(F3_BUTTON);

                                break;
                            }

                            else if (strstr(currentFile->extension, "SHL") == 0) {
                                VGA_clrscr();
                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                
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
                                
                                printf("\n\nPress [F3] to exit");
                                keyboard_wait(F3_BUTTON);
                                
                                break;
                            }
                        }

                        else if (action_type == BACKSPACE_BUTTON) {
                            printf("\nDelete? (Y/N): ");
                                            
                            char* user_choose = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
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
    set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
    printf("Directory: [%s]\n%s%s%s", FAT_get_current_path(), LINE, HEADER, LINE);

    int rows = 0;
    if (rows++ == row_position) cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, UPPED_DIR);
    else printf(UPPED_DIR);

    struct FATDirectory* currentDir = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(current_directory->directory->directory_meta), NULL, FALSE)->subDirectory;
    while (currentDir != NULL) {
        char name[COLUMN_WIDTH + 1];
        memset(name, ' ', COLUMN_WIDTH);
        name[COLUMN_WIDTH] = '\0';

        int name_length = strlen(currentDir->name);
        if (name_length <= COLUMN_WIDTH) strncpy(name, currentDir->name, name_length);
        else strncpy(name, currentDir->name, COLUMN_WIDTH - 3);

        if (rows++ == row_position) cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, "| %s | Dir               | N/A       | N/A       | N/A           |\n", name);
        else printf("| %s | Dir               | N/A       | N/A       | N/A           |\n", name);

        currentDir = currentDir->next;
    }

    struct FATFile* currentFile = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(current_directory->directory->directory_meta), NULL, FALSE)->files;
    while (currentFile != NULL) {
        ////////////////
        //  FILE NAME PREPARATIONS
        //

            char file_name[COLUMN_WIDTH + 1];
            memset(file_name, ' ', COLUMN_WIDTH);
            file_name[COLUMN_WIDTH] = '\0';

            int file_name_length = strlen(currentFile->name);
            if (file_name_length <= COLUMN_WIDTH) strncpy(file_name, currentFile->name, file_name_length);
            else strncpy(file_name, currentFile->name, COLUMN_WIDTH - 3);
        
        //
        //  FILE NAME PREPARATIONS
        ////////////////

        ////////////////
        //  FILE EXTENSION PREPARATIONS
        //

            char file_extension[COLUMN_WIDTH  - 5];
            memset(file_extension, ' ', COLUMN_WIDTH - 6);
            file_extension[COLUMN_WIDTH - 6] = '\0';

            int file_extension_length = strlen(currentFile->extension);
            if (file_extension_length <= COLUMN_WIDTH - 6) strncpy(file_extension, currentFile->extension, file_extension_length);
            else strncpy(file_extension, currentFile->extension, COLUMN_WIDTH - 9);
        
        //
        //  FILE EXTENSION PREPARATIONS
        ////////////////

        ////////////////
        //  FILE SIZE PREPARATIONS
        //

            char file_size[COLUMN_WIDTH - 1];
            memset(file_size, ' ', COLUMN_WIDTH - 2);
            file_size[COLUMN_WIDTH - 2] = '\0';

            char* file_size_str = itoa(currentFile->file_meta.file_size); // Mem leak?
            strcat(file_size_str, "B");

            int file_size_length = strlen(file_size_str);
            if (file_size_length <= COLUMN_WIDTH - 2) strncpy(file_size, file_size_str, file_size_length);
            else strncpy(file_size, file_size_str, (COLUMN_WIDTH - 2) - 3);

        //
        //  FILE SIZE PREPARATIONS
        ////////////////

        uint8_t foreground_color = FOREGROUND_BRIGHT_WHITE;
        if (strstr(file_extension, "TXT") == 0)
            foreground_color = FOREGROUND_YELLOW;
        else if (strstr(file_extension, "ASM") == 0)
            foreground_color = FOREGROUND_GREY;
        else if (strstr(file_extension, "SHL") == 0)
            foreground_color = FOREGROUND_GREEN;
        else if (strstr(file_extension, "OBJ") == 0)
            foreground_color = FOREGROUND_BLACK;

        if (rows++ == row_position) cprintf(BACKGROUND_RED + foreground_color, "| %s | File              | %d %d %d     | %s | %s |\n",
                file_name, currentFile->read_level, currentFile->write_level, currentFile->edit_level, file_extension, file_size);
        else cprintf(BACKGROUND_BLUE + foreground_color, "| %s | File              | %d %d %d     | %s | %s |\n",
                file_name, currentFile->read_level, currentFile->write_level, currentFile->edit_level, file_extension, file_size);

        currentFile = currentFile->next;
        free(file_size_str);  // Mem leak?
    }

    if (row_position > rows) row_position = rows - 1;

    for (int i = 0; i < 15 - rows; i++) printf(EMPTY);
    printf("%s[F1 - CREATE DIR]   [F2 - CREATE FILE]   [F3 - EXIT]   [F4 - EDIT]\n[ENTER - INTERACT]   [BACKSPACE - DELETE]\n", LINE);
}