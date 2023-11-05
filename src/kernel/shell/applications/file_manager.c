#include "../include/file_manager.h"

int row_position = 1;

void open_file_manager() {
    VGA_clrscr();
    print_directory_data();

    while(1) {
        char user_action = keyboard_navigation();
        if (user_action == '\4' && row_position > 0) 
            row_position--;
        else if (user_action == '\3')
            row_position++;
        else if (user_action == '\n') 
            execute_item();
        else if (user_action == '\5') {
            VGA_clrscr();
            break;
        }

        VGA_clrscr();
        print_directory_data();
    }
}

void execute_item() {
    if (row_position == 0) {
        up_from_directory();
        return;
    }
    
    int rows = 1;
    struct Directory* currentDir = get_current_directory()->subDirectory;
    while (currentDir != NULL) {
        if (rows++ == row_position) {
            move_to_directory(currentDir->name);
            break;
        }

        currentDir = currentDir->next;
    }

    struct File* currentFile = currentDir->files;
    while (currentFile != NULL) {
        if (rows++ == row_position) {

            break;
        }

        currentFile = currentFile->next;
    }

}

void print_directory_data() {
    printf("\n");
    printf("+-----------------+-------+-----------+-----------------+\n");
    printf("| Name            | Type  | Access    | Size            |\n");
    printf("+-----------------+-------+-----------+-----------------+\n");

    int rows = 0;
    if (rows++ == row_position) {
        cprintf(FOREGROUND_GREEN, ">");
    } else printf("|");

    printf("       ...       | None  | N/A       | N/A             |\n");

    struct Directory* currentDir = get_current_directory()->subDirectory;
    while (currentDir != NULL) {
        char name[COLUMN_WIDTH + 1];
        memset(name, ' ', COLUMN_WIDTH);
        name[COLUMN_WIDTH] = '\0';

        int name_length = strlen(currentDir->name);
        if (name_length <= COLUMN_WIDTH) strncpy(name, currentDir->name, name_length);
        else strncpy(name, currentDir->name, COLUMN_WIDTH - 3);

        if (rows++ == row_position) {
            cprintf(FOREGROUND_GREEN, ">");
        } else printf("|");

        printf(" %s | Dir   | N/A       | N/A             |\n", name);

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
        
        char file_size[COLUMN_WIDTH + 1];
        memset(file_size, ' ', COLUMN_WIDTH);
        file_size[COLUMN_WIDTH] = '\0';

        char* file_size_str = fprintf_unsigned(-1, currentFile->sector_count * SECTOR_SIZE, 10);
        reverse(file_size_str, strlen(file_size_str));

        int file_size_length = strlen(file_size_str);
        if (file_size_length <= COLUMN_WIDTH) strncpy(file_size, file_size_str, file_size_length);
        else strncpy(file_size, file_size_str, COLUMN_WIDTH - 3);
        
        if (rows++ == row_position) {
            cprintf(FOREGROUND_GREEN, ">");
        } else printf("|");

        printf(" %s | File  | %d         | %s |\n",
            file_name,
            currentFile->fileType,
            file_size);

        currentFile = currentFile->next;
    }

    printf("+-----------------+-------+-----------+-----------------+\n");
}