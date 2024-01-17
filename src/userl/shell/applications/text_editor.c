#include "../include/text_editor.h"
// complete porting

void cursor_place_to_line() {
    int coords[2];
    SYS_get_cursor(&coords);
    while (SYS_scrchar(coords[0] - 1, coords[1]) == NULL) {
        SYS_set_cursor(coords[0] - 1, coords[1]);
        SYS_get_cursor(&coords);
    }
}

char* edit_file;
void update_screen(char* data, int x_position, int y_position) {
    SYS_clrs();
    printf("%sFile: [%s]   [F1 - SAVE]   [F3 - EXIT]\n%s", LINE, edit_file, LINE);
    printf("%s", data);

    SYS_set_cursor(x_position, y_position);
}

void get_start_positions(char* text, int* x_position, int* y_position) {
    *x_position = 0;  // Reset x_position to 0
    *y_position = 0;  // Reset y_position to 0

    int pos = 0;

    int length = strlen(text);
    while (pos < length) {
        if (text[pos] == '\n') {
            (*y_position)++;
            *x_position = 0;  // Reset x_position to 0 for the next line
        } else  (*x_position)++;
        
        pos++;
    }
}

void get_line_end(char* text, int* x_position, int y_position, char delim) {
    *x_position = 1;  // Reset x_position to 0

    int pos = 0;
    int length = strlen(text);
    int y = 0;

    while (pos < length) {
        if (text[pos] == delim || text[pos] == '\n') {
            if (y++ == y_position) 
                break;
            
            *x_position = 0;  // Reset x_position to 0 for the next line
        } else  (*x_position)++;
        
        pos++;
    }
}

int character_position = 0;
int y_cursor, x_cursor = 0;
char* keyboard_edit(char* previous_data, int color) {
    char* input = (char*)SYS_malloc(strlen(previous_data));
    memset(input, 0, sizeof(input));
    strcpy(input, previous_data);
    SYS_free(previous_data);

    y_cursor, x_cursor = 0;
    get_start_positions(input, &x_cursor, &y_cursor);

    cprintf(color, "%s", input);
    size_t input_size = strlen(input);

    while (1) {
        char currentCharacter = SYS_keyboard_wait_key();
        character_position++;

        if (currentCharacter == F1_BUTTON) return input;
        if (currentCharacter != F3_BUTTON) {
            int coords[2];
            SYS_get_cursor(&coords);
            if (currentCharacter == LEFT_ARROW_BUTTON) { // Left
                if (coords[0] - 1 >= 0) {
                    SYS_set_cursor(coords[0] - 1, coords[1]);
                    x_cursor--;
                }

                continue;
            }

            else if (currentCharacter == RIGHT_ARROW_BUTTON) { // Right
                if (SYS_scrchar(coords[0], coords[1]) != NULL && coords[0] < 80) {
                    SYS_set_cursor(coords[0] + 1, coords[1]); 
                    x_cursor++;
                }

                continue;
            }

            else if (currentCharacter == DOWN_ARROW_BUTTON) { // Down
                if (SYS_scrchar(coords[0], coords[1] + 1) != NULL) {
                    SYS_set_cursor(coords[0], coords[1] + 1); 
                    y_cursor++;
                }

                continue;
            }

            else if (currentCharacter == UP_ARROW_BUTTON) { // Up
                if (SYS_scrchar(coords[0], coords[1] - 1) != NULL) {
                    SYS_set_cursor(coords[0], coords[1] - 1); 
                    y_cursor--;
                }

                continue;
            }

            else if (currentCharacter == BACKSPACE_BUTTON) {
                if (strlen(input) > 0 && input_size > 0) {
                    if (place_char_in_text(input, '\253', x_cursor, y_cursor) == '\n') {
                        SYS_set_cursor(80, coords[1] - 1);
                        cursor_place_to_line();
                        get_line_end(input, &x_cursor, --y_cursor, '\253');
                    } else {
                        if (--x_cursor < 0) {
                            get_line_end(input, &x_cursor, --y_cursor, '\n');
                            SYS_set_cursor(80, coords[1] - 1);
                            cursor_place_to_line();
                        } else SYS_set_cursor(coords[0] - 1, coords[1]);
                    }

                    SYS_set_cursor(coords[0], coords[1]);
                    SYS_set_scrchar(coords[0], coords[1], NULL);

                    input_size--;
                    fit_string(input, strlen(input), '\253');

                    update_screen(input, coords[0], coords[1]);
                }

                continue;
            }

            else if (currentCharacter == ENTER_BUTTON) {
                cprintf(color, "\n");                        
                add_char_to_string(&input, ++input_size, '\n');

                x_cursor = 0;
                y_cursor++;
            }

            else {
                cprintf(color, "%c", currentCharacter);
                add_char_to_string(&input, ++input_size, currentCharacter);

                if (++x_cursor >= 80) {
                    x_cursor = 0;
                    y_cursor++;
                }
            }
        }
        else return previous_data;
    }

    return input;
}

void text_editor_init(char* path, int color) {
    SYS_clrs();

    edit_file = path;
    char* file_text = SYS_fread(path);
    printf("%sFile: [%s]   [F1 - SAVE]   [F3 - EXIT]\n%s", LINE, path, LINE);
    SYS_scrclr(BACKGROUND_BLACK + FOREGROUND_BRIGHT_WHITE);
    SYS_fwrite(path, keyboard_edit(file_text, color));
    
    SYS_free(file_text);
}