#include "editor.h"

// TODO:
// - Cursor show
// - Text scroll
// - Text edit

Content* edit_content;
File* edit_file;

char* edit_path;
int current_line = 0;


int main(int argc, char *argv[]) {
    if (argc != 0) edit_path = argv[0];
    else {
        mkfile("home", "tfile.txt");
        edit_path = "home\\tfile.txt";
    }
    
    current_line = 2;
    edit_content = get_content(edit_path);

    display_text();
    return loop();
}

int loop() {
    while (1) {
        char action = get_char();
        if (action == DOWN_ARROW_BUTTON) {
            current_line++;
            display_text();
        }

        else if (action == UP_ARROW_BUTTON) {
            if (current_line - 1 < 0) continue;

            current_line--;
            display_text();
        }

        else if (action == F3_BUTTON) return 0;
        else if (action == F2_BUTTON) {
            fwrite(edit_path, NULL);
            return 1;
        }

        else if (action == F1_BUTTON) fwrite(edit_path, NULL);
    }
}

// Save text to editor buffer (prefer static buffer allocation)
void display_text() {
    clrscr();

    int current_height = 0;
    int offset = 0;

    while (current_height++ < current_line) {
        char buffer[CHARS_ON_LINE] = { '\0' };
        char stop[1] = { '\n' };

        fread_off_stop(edit_content, offset, buffer, CHARS_ON_LINE, stop);
        offset += strlen(buffer);

        printf(buffer);
    }

    // for (int i = 0; i < SCREEN_HEIGHT; i++) {
    //     char text2display[CHARS_ON_LINE] = { '\0' };
    //     char stop[1] = { '\n' };

    //     fread_off_stop(edit_content, offset, text2display, CHARS_ON_LINE, stop);
    //     offset += strlen(text2display);

    //     printf(text2display);
    // }
    
    cursor_set(0, SCREEN_HEIGHT);
    printf("| F3 - EXIT | F1 - SAVE & EXIT | F2 - SAVE |");
    cursor_set(0, 1);
}