#include <stdint.h>

#include "../include/file_manager.h"

int row_position = 1;

void open_file_manager(struct User* user) {
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
            
            create_directory(dir_name);   
            free(dir_name); 
        }

        else if (user_action == F2_BUTTON) {
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nFile name: ");
            char* file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
            
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\t\tFile type [XXX]: ");
            char* file_type = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
            
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\t\tFile extension: ");
            char* file_extension = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

            create_file(file_type[0] - '0', file_type[1] - '0', file_type[2] - '0', file_name, file_extension, ATA_find_empty_sector(FILES_SECTOR_OFFSET));  
            free(file_name);
            free(file_type);
            free(file_extension);
        }

        else if (user_action == F4_BUTTON) execute_item(user, user_action);
        
        VGA_text_clrscr();
        print_directory_data();
    }
}

void execute_item(struct User* user, char action_type) {
    if (row_position == 0 && action_type != BACKSPACE_BUTTON) {
        up_from_directory();
        return;
    }
    
    ///////////////////////////////////
    //  SELECTED DIRECTORY
    //

        int rows = 1;
        struct Directory* currentDir = get_current_directory()->subDirectory;
        while (currentDir != NULL) {
            if (rows++ == row_position) {
                if (action_type == ENTER_BUTTON)
                    move_to_directory(currentDir->name);

                else if (action_type == BACKSPACE_BUTTON) {       
                    if ((currentDir->subDirectory != NULL || currentDir->files != NULL) && user->edit_access != 0)
                        break;

                    printf("\nDelete? (Y/N): ");
                    while (1) {
                        char* answer = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                        if (strcmp(answer, "y") == 0) {
                            delete_directory(currentDir->name);
                            free(answer);
                            break;
                        }
                        
                        free(answer);
                        break;
                    }
                }

                else if (action_type == F4_BUTTON) { 
                    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew dir name: ");
                    char* new_dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                    if (find_directory(new_dir_name) == NULL) {
                        memset(currentDir->name, ' ', sizeof(currentDir->name));
                        strncpy(currentDir->name, new_dir_name, 11);

                        save_file_system();
                        break;
                    }

                    free(new_dir_name);
                }

                break;
            }

            currentDir = currentDir->next;
        }

    //
    //  SELECTED DIRECTORY
    ///////////////////////////////////

    ///////////////////////////////////
    //  SELECTED FILE
    //

        struct File* currentFile = get_current_directory()->files;
        while (currentFile != NULL) {
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
                                        if (user->write_access <= currentFile->write_level) 
                                            text_editor_init(currentFile, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                        
                                    break;

                                    case RENAME_POS:
                                        if (user->edit_access <= currentFile->edit_level) {
                                            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew file name: ");
                                            char* new_file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                            
                                            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew file type [XXX]: ");
                                            char* new_file_type = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                            
                                            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nNew file extension: ");
                                            char* new_file_extension = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                                            if (file_exist(new_file_name) != 1) {
                                                memset(currentFile->name, ' ', sizeof(currentFile->name));
                                                strncpy(currentFile->name, new_file_name, 11);

                                                memset(currentFile->extension, ' ', sizeof(currentFile->extension));
                                                strncpy(currentFile->extension, new_file_extension, 3);

                                                currentFile->read_level     = new_file_type[0] - '0';
                                                currentFile->write_level    = new_file_type[1] - '0';
                                                currentFile->edit_level     = new_file_type[2] - '0';

                                                save_file_system();
                                            }
                                            
                                            free(new_file_name);
                                            free(new_file_type);
                                            free(new_file_extension);
                                        }
                                        
                                    break;

                                    case DELETE_POS:
                                        if (user->edit_access <= currentFile->edit_level) {
                                            printf("\nDelete? (Y/N): ");
                                            
                                            char* user_choose = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                            if (strcmp(user_choose, "y") == 0) delete_file(currentFile->name);

                                            free(user_choose);
                                        }
                                        
                                        VGA_clrscr();

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

                    if (strstr(currentFile->extension, "txt") == 0) {
                        if (user->read_access <= currentFile->read_level) {
                            VGA_clrscr();
                            printf("%sFile: [%s]   [F3 - EXIT]\n%s\n\n\n%s\n\n%s", LINE, currentFile->name, LINE, read_file(currentFile), LINE);
                            set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                            keyboard_wait(F3_BUTTON);
                        }

                        break;
                    }

                    else if (strstr(currentFile->extension, "asm") == 0) {
                        VGA_clrscr();
                        asm_execute(read_file(currentFile));
                        set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

                        printf("\n\nPress [F3] to exit");
                        keyboard_wait(F3_BUTTON);

                        break;
                    }

                    else if (strstr(currentFile->extension, "shl") == 0) {
                        VGA_clrscr();
                        set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                        
                        char* sector_data = read_file(currentFile);
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

                //
                //  EXECUTE
                /////////////////

                break;
            }

            currentFile = currentFile->next;
        }

    //
    //  SELECTED DIRECTORY
    ///////////////////////////////////

}

void print_directory_data() {
    set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

    char* directory_name = get_full_temp_name();  // Mem leak?
    printf("Directory: [%s]\n%s%s%s", directory_name, LINE, HEADER, LINE);
    free(directory_name);  // Mem leak?

    int rows = 0;
    if (rows++ == row_position) cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, UPPED_DIR);
    else printf(UPPED_DIR);

    struct Directory* currentDir = get_current_directory()->subDirectory;
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

    struct File* currentFile = get_current_directory()->files;
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

            char* file_size_str = itoa(currentFile->sector_count * SECTOR_SIZE); // Mem leak?
            strcat(file_size_str, "B");

            int file_size_length = strlen(file_size_str);
            if (file_size_length <= COLUMN_WIDTH - 2) strncpy(file_size, file_size_str, file_size_length);
            else strncpy(file_size, file_size_str, (COLUMN_WIDTH - 2) - 3);

        //
        //  FILE SIZE PREPARATIONS
        ////////////////

        uint8_t foreground_color = FOREGROUND_BRIGHT_WHITE;
        if (strstr(file_extension, "txt") == 0)
            foreground_color = FOREGROUND_YELLOW;
        else if (strstr(file_extension, "asm") == 0)
            foreground_color = FOREGROUND_GREY;
        else if (strstr(file_extension, "shl") == 0)
            foreground_color = FOREGROUND_GREEN;
        else if (strstr(file_extension, "obj") == 0)
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