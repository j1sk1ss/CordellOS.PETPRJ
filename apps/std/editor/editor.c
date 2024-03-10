#include "editor.h"

// TODO:
// - Cursor show
// - Text scroll
// - Text edit

Content* edit_content;
File* edit_file;

int current_line = 0;


int main(int argc, char *argv[]) {
    if (argc != 0) edit_content = get_content(argv[0]);
    else edit_content = FSLIB_create_content("tfile", 0, "txt");
    
    edit_file = edit_content->file;
    current_line = 0;
    display_text();
    loop();
}

void loop() {
    while (1) {
        char action = get_char();
        if (action == DOWN_ARROW_BUTTON) {
            if (current_line + 1 * CHARS_ON_LINE > edit_file->file_meta.file_size)  continue;

            current_line++;
            display_text();
        }
        else if (action == UP_ARROW_BUTTON) {
            if (current_line - 1 < 0) continue;

            current_line--;
            display_text();
        }
        else if (action == F3_BUTTON) {
            // exit
        }
        else if (action == F2_BUTTON) {
            // save
        }
        else if (action == F1_BUTTON) {
            // save&exit
        }
    }
}

void display_text() {
    clrscr();

    char text2display[SCREEN_SIZE] = { '\0' }; 
    fread_off(edit_content, 0, text2display, min(SCREEN_SIZE - (3 * CHARS_ON_LINE), edit_file->file_meta.file_size));

    printf("FILE [%s.%s] | SIZE [%i]\n%s\n", edit_file->name, edit_file->extension, edit_file->file_meta.file_size, text2display);

    cursor_set(0, SCREEN_HEIGHT);
    printf("F3 - EXIT | F1 - SAVE&EXIT | F2 - SAVE");
    cursor_set(0, 1);
}