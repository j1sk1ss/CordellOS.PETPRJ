#include "../include/file_manager.h"

int row_position = 1;

void open_file_manager(int access_level) {
    VGA_clrscr();
    print_directory_data();

    while(1) {
        char user_action = keyboard_navigation();
        if (user_action == '\4' && row_position > 0) 
            row_position--;
        else if (user_action == '\3')
            row_position++;
        else if (user_action == '\n' || user_action == '\b') 
            execute_item(access_level);
        else if (user_action == '\5') {
            VGA_clrscr();
            break;
        }
        else if (user_action == '\6') {
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nDir name: ");
            char* dir_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

            create_directory(dir_name);    
        }
        else if (user_action == '\7') {
            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\nFile name: ");
            char* file_name = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

            cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "\tFile type: ");
            char* file_type = keyboard_read(VISIBLE_KEYBOARD, BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

            create_file(atoi(file_type), file_name, ATA_find_empty_sector());  
        }

        VGA_clrscr();
        print_directory_data();
    }
}

void execute_item(int access_level, char action_type) {
    if (row_position == 0 && action_type != '\b') {
        up_from_directory();
        return;
    }
    
    int rows = 1;
    struct Directory* currentDir = get_current_directory()->subDirectory;
    while (currentDir != NULL) {
        if (rows++ == row_position) {
            if (action_type != '\b')
                move_to_directory(currentDir->name);
            else if (access_level == 0 || (access_level == 1 && (currentDir->subDirectory == NULL && currentDir->files == NULL)))
                delete_directory(currentDir->name);

            break;
        }

        currentDir = currentDir->next;
    }

    struct File* currentFile = get_current_directory()->files;
    while (currentFile != NULL) {
        if (rows++ == row_position) {
            row_position = 0;
            while(1) {
                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
            
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE,"\n");
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

                if (row_position == READ_POS) {
                    cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, READ_LINE);
                } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, READ_LINE);
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

                if (row_position == EDIT_POS) {
                    cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, EDIT_LINE);
                } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, EDIT_LINE);
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

                if (row_position == ASM_POS) {
                    cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, ASM_LINE);
                } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, ASM_LINE);
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

                if (row_position == DELETE_POS) {
                    cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, DELETE_LINE);
                } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, DELETE_LINE);
                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

                char user_action = keyboard_navigation();
                if (user_action == '\4' && row_position > 0) 
                    row_position--;
                else if (user_action == '\3' && row_position < 4)
                    row_position++;
                else if (user_action == '\n') {
                    if (currentFile->fileType >= access_level) {
                        switch (row_position) {
                            case READ_POS:
                                VGA_clrscr();
                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "Press [ENTER] to exit\n\nFile: [%s]\n\n%s",
                                            currentFile->name, read_file(currentFile));

                                while (1) {
                                    user_action = keyboard_navigation();
                                    if (user_action == '\n')
                                        break;
                                }
                                
                            break;

                            case EDIT_POS:
                                VGA_clrscr();
                                set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);
                                cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "Stai modificando il file. Utilizzare CAPSLOCK per uscire.\r\n\r\n");

                                write_file(currentFile, keyboard_edit(read_file(currentFile), BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE));
                            break;

                            case ASM_POS:
                                VGA_clrscr();
                                asm_execute(read_file(currentFile));

                                printf("\nPress ENTER to exit");
                                while (1) {
                                    user_action = keyboard_navigation();
                                    if (user_action == '\n')
                                        break;
                                }
                            break;

                            case DELETE_POS:
                                VGA_clrscr();
                                if (access_level <= currentFile->fileType) 
                                    delete_file(currentFile->name);
                                
                            break;
                        }

                        if (row_position == 0)
                            break;
                    }
                }
                else if (user_action == '\5') {
                    VGA_clrscr();
                    break;
                }

                VGA_clrscr();
            }

            break;
        }

        currentFile = currentFile->next;
    }

}

void print_directory_data() {
    set_color(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE);

    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "Directory: [%s]\n", get_full_temp_name());
    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);
    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, HEADER);
    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);

    int rows = 0;
    if (rows++ == row_position) {
        cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, UPPED_DIR);
    } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, UPPED_DIR);

    struct Directory* currentDir = get_current_directory()->subDirectory;
    while (currentDir != NULL) {
        char name[COLUMN_WIDTH + 1];
        memset(name, ' ', COLUMN_WIDTH);
        name[COLUMN_WIDTH] = '\0';

        int name_length = strlen(currentDir->name);
        if (name_length <= COLUMN_WIDTH) strncpy(name, currentDir->name, name_length);
        else strncpy(name, currentDir->name, COLUMN_WIDTH - 3);

        if (rows++ == row_position) {
            cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, "| %s | Dir               | N/A       | N/A                       |\n", name);
        } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "| %s | Dir               | N/A       | N/A                       |\n", name);

        currentDir = currentDir->next;
    }

    struct File* currentFile = get_current_directory()->files;
    while (currentFile != NULL) {
        char file_name[COLUMN_WIDTH + 1];
        memset(file_name, ' ', COLUMN_WIDTH);
        file_name[COLUMN_WIDTH] = '\0';

        int file_name_length = strlen(currentFile->name);
        if (file_name_length <= COLUMN_WIDTH) strncpy(file_name, currentFile->name, file_name_length);
        else strncpy(file_name, currentFile->name, COLUMN_WIDTH - 3);
        
        char file_size[COLUMN_WIDTH + 10 + 1];
        memset(file_size, ' ', COLUMN_WIDTH + 10);
        file_size[COLUMN_WIDTH + 10] = '\0';

        char* file_size_str = fprintf_unsigned(-1, currentFile->sector_count * SECTOR_SIZE, 10, 0);
        reverse(file_size_str, strlen(file_size_str));

        int file_size_length = strlen(file_size_str);
        if (file_size_length <= COLUMN_WIDTH + 10) strncpy(file_size, file_size_str, file_size_length);
        else strncpy(file_size, file_size_str, (COLUMN_WIDTH + 10) - 3);
        
        if (rows++ == row_position) {
            cprintf(BACKGROUND_RED + FOREGROUND_BRIGHT_WHITE, "| %s | File              | %d         | %s |\n",
                file_name,
                currentFile->fileType,
                file_size);
        } else cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "| %s | File              | %d         | %s |\n",
                file_name,
                currentFile->fileType,
                file_size);

        currentFile = currentFile->next;
    }

    if (row_position > rows)
        row_position = rows - 1;

    for (int i = 0; i < 16 - rows; i++)
        cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, EMPTY);

    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, LINE);
    cprintf(BACKGROUND_BLUE + FOREGROUND_BRIGHT_WHITE, "[F1 - CREATE DIR]   [F2 - CREATE FILE]");
}