#include "../include/text_editor.h"

char* edit_file;
void update_screen(char* data, int x_position, int y_position) {
    VGA_text_clrscr();
    printf("%sFile: [%s]   [F1 - SAVE]   [F3 - EXIT]\n%s", LINE, edit_file, LINE);
    printf("%s", data);

    VGA_setcursor(x_position, y_position);
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
    char* input = (char*)malloc(strlen(previous_data));
    memset(input, 0, sizeof(input));
    strcpy(input, previous_data);
    free(previous_data);

    y_cursor, x_cursor = 0;
    get_start_positions(input, &x_cursor, &y_cursor);

    cprintf(color, "%s", input);
    size_t input_size = strlen(input);

    while (1) {
        if (i686_inb(0x64) & 0x1) {
            char character = i686_inb(0x60);

            if (!(character & 0x80)) {
                character_position++;

                char currentCharacter = get_character(character);
                if (currentCharacter == F1_BUTTON) return input;
                if (currentCharacter != F3_BUTTON) {
                    if (currentCharacter == LEFT_ARROW_BUTTON) { // Left
                        if (VGA_cursor_get_x() - 1 >= 0) {
                            VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y());
                            x_cursor--;
                        }

                        continue;
                    }

                    else if (currentCharacter == RIGHT_ARROW_BUTTON) { // Right
                        if (VGA_getchr(VGA_cursor_get_x(), VGA_cursor_get_y()) != NULL && VGA_cursor_get_x() < 80) {
                            VGA_setcursor(VGA_cursor_get_x() + 1, VGA_cursor_get_y()); 
                            x_cursor++;
                        }

                        continue;
                    }

                    else if (currentCharacter == DOWN_ARROW_BUTTON) { // Down
                        if (VGA_getchr(VGA_cursor_get_x(), VGA_cursor_get_y() + 1) != NULL) {
                            VGA_setcursor(VGA_cursor_get_x(), VGA_cursor_get_y() + 1); 
                            y_cursor++;
                        }

                        continue;
                    }

                    else if (currentCharacter == UP_ARROW_BUTTON) { // Up
                        if (VGA_getchr(VGA_cursor_get_x(), VGA_cursor_get_y() - 1) != NULL) {
                            VGA_setcursor(VGA_cursor_get_x(), VGA_cursor_get_y() - 1); 
                            y_cursor--;
                        }

                        continue;
                    }

                    else if (currentCharacter == BACKSPACE_BUTTON) {
                        if (strlen(input) > 0 && input_size > 0) {
                            if (place_char_in_text(input, '\253', x_cursor, y_cursor) == '\n') {
                                VGA_setcursor(80, VGA_cursor_get_y() - 1);
                                VGA_cursor_place_to_line();
                                get_line_end(input, &x_cursor, --y_cursor, '\253');
                            } else {
                                if (--x_cursor < 0) {
                                    get_line_end(input, &x_cursor, --y_cursor, '\n');
                                    VGA_setcursor(80, VGA_cursor_get_y() - 1);
                                    VGA_cursor_place_to_line();
                                } else VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y());
                            }

                            VGA_setcursor(VGA_cursor_get_x(), VGA_cursor_get_y());
                            VGA_putchr(VGA_cursor_get_x(), VGA_cursor_get_y(), NULL);

                            input_size--;
                            fit_string(input, strlen(input), '\253');

                            update_screen(input, VGA_cursor_get_x(), VGA_cursor_get_y());
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
        }
    }

    return input;
}

void text_editor_init(char* path, int color) {
    VGA_clrscr();

    edit_file = path;
    char* file_text = FAT_read_content(FAT_get_content(path));
    printf("%sFile: [%s]   [F1 - SAVE]   [F3 - EXIT]\n%s", LINE, path, LINE);
    set_color(BACKGROUND_BLACK + FOREGROUND_BRIGHT_WHITE);
    FAT_edit_content(path, keyboard_edit(file_text, color));
    
    free(file_text);
}