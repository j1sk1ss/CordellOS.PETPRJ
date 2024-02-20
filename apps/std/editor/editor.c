#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <fatlib.h>
#include <memory.h>


#define ENDOFLINE  80
#define SPACE      0x20  // ASCII space
#define LEFTARROW  0x4B	 // Keyboard scancodes...
#define RIGHTARROW 0x4D
#define UPARROW    0x48
#define DOWNARROW  0x50
#define HOMEKEY    0x47
#define ENDKEY     0x4F
#define DELKEY     0x53
#define CREATENEW  'c'   // Editor screen keybinds
#define LOADCURR   'l'
#define BINFILE    'b'
#define OTHERFILE  'o'
#define RUNINPUT   '$'   // Hex editor keybinds
#define ENDPGM     '?'
#define SAVEPGM    'S'


enum file_modes {
    NEW    = 1,     // New file
    UPDATE = 2      // Updating an existing file
};


uint8_t editor_filetype[3];     // Global variables
uint8_t editor_filename[10];
uint32_t editor_filesize = 0;

uint8_t input_char;

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

struct UFATContent* content;
struct UFATFile* file;

uint8_t* file_ptr;
uint32_t file_offset;
uint32_t file_address;

uint32_t current_line_length;
uint32_t file_length_lines;
uint32_t file_length_bytes;

uint8_t hex_count = 0;
uint8_t hex_byte = 0;
uint8_t file_mode;     


int main(int argc, char *argv[]) {
    memset("\n", ' ', 79);
    "\n"[79] = '\0';

    if (argc < 2) {
        clrscr();

        file_mode = NEW;
        cursor_x  = 0;
        cursor_y  = 0;

        printf("\t\t\t(B)inary/hex file or (O)ther file type (.txt, etc)?\t[?]");

        editor_filesize = 0;
        input_char = wait_char();
        while (input_char != BINFILE && input_char != OTHERFILE)
            input_char = wait_char();

        clrscr();
        cursor_x = 0;
        cursor_y = 0;

        file_ptr = (uint8_t*)malloc(512);
        file_address = (uint32_t)file_ptr;
        file_offset = 0;

        if (input_char == BINFILE) return -1;
        else {
            write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line"); 

            memset(file_ptr, 0, 512);

            cursor_x = 0;
            cursor_y = 0;
            current_line_length = 0;
            file_length_lines = 0;
            file_length_bytes = 0;

            text_editor();
        }
    
    }
    else {
        file_mode = UPDATE;
        editor_load_file(argv[0]); 
    }

    return 0;
}

void editor_load_file(char* file_address) {
    if (cexists(file_address) == 0) {
        write_bottom_screen_message("Load file error occurred, press any key to go back...");
        input_char = wait_char();
        clrscr();

        cursor_x = 0;
        cursor_y = 0;

        return;        
    }

    content = get_content(file_address);
    file = content->file;
    if (file == NULL) return;

    strncpy(editor_filename, file->file_meta.file_name, 10);
    clrscr();

    cursor_x            = 0;
    cursor_y            = 0;
    current_line_length = 0;
    file_length_lines   = 0;
    file_length_bytes   = 0;

    while (file_length_bytes < file->file_meta.file_size) {
        uint32_t print_size = min(file->file_meta.file_size - file_length_bytes, 64);
        char* print_buffer = calloc(print_size, 1);

        fread_off(content, file_length_bytes, print_buffer, print_size);
        printf(print_buffer);
        free(print_buffer);

        file_offset += print_size;
        file_length_bytes += print_size;
    }

    write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line");

    file_offset = 0;
    cursor_x    = 0;
    cursor_y    = 0;

    cursor_set(cursor_x, cursor_y);
    current_line_length = 0;

    char* line_buffer = calloc(100, 1);
    fread_off(content, 0, line_buffer, 100);
    for (uint32_t i = 0; line_buffer[i] != '\n' && file_offset != 99; i++) { 
        file_offset++;
        current_line_length++;
    }

    free(line_buffer);
    current_line_length++;
    file_offset = 0;

    text_editor();
}

void text_editor() {
    uint16_t save_x, save_y;
    uint8_t* save_file_ptr;
    uint8_t save_file_offset;
    uint8_t changed_filename[10];
    uint8_t unsaved = 0;

    write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line");
    cursor_x = 0;
    cursor_y = 0;
    cursor_set(cursor_x, cursor_y);

    while (1) {
        int cursor[2];
        cursor_get(cursor);

        save_x   = cursor[0];
        save_y   = cursor[1];
        cursor_x = 1;
        cursor_y = (get_resolution_y() / 16) - 2;

        printf("X: %i\tY: %i\tLEN: %i\tSIZE: %i\t", cursor_x, cursor_y, current_line_length, file_length_bytes);
        printf("[%s.%s]  ", file->name, file->extension);
        if (unsaved) putc('*');

        if (file_mode == NEW) printf("NEW");
        else if (file_mode == UPDATE) printf("UPD");

        cursor_x = save_x;
        cursor_y = save_y;
        cursor_set(cursor_x, cursor_y);

        input_char = wait_char();
        if (input_char == F4_BUTTON) return; 
        else if (input_char == F1_BUTTON) { // CTRLS Save file to disk
            save_x = cursor_x;
            save_y = cursor_y;

            if (file_mode == NEW) {
                remove_cursor(cursor_x, cursor_y);  // Erase cursor first

                write_bottom_screen_message("\nEnter file name:");
                cursor_set(cursor_x, cursor_y);

                input_file_name();
                write_bottom_screen_message("\nEnter file extension:");
                cursor_set(cursor_x, cursor_y);

                for (uint8_t i = 0; i < 3; i++) 
                    cursor_set(cursor_x, cursor_y);

                file_mode = UPDATE;
                write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line\0");
            }

            // Call save file
            // file name, file type, file size, address to save from
            // TODO: Use actual file size, not hardcoded 0x0001
            if (!save_file(editor_filename, editor_filetype, 0x0001, file_address)) {
                write_bottom_screen_message("Save file error occurred");   // Errored on save_file()

            } else {
                write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line\0");  // Write keybinds at bottom

                cursor_x = save_x;
                cursor_y = save_y;
                cursor_set(cursor_x, cursor_y);
            }

            unsaved = 0;    // User saved file, no more unsaved changes

            continue;
        }

        // Ctrl-C: Change file name & extension
        if (input_char == F2_BUTTON) {
            save_x = cursor_x;
            save_y = cursor_y;
            remove_cursor(cursor_x, cursor_y);  // Erase cursor

            write_bottom_screen_message("\n");
            write_bottom_screen_message("Enter file name: \0");   // Enter file name 
            cursor_set(cursor_x, cursor_y);

            // Get old file name
            strncpy(changed_filename, editor_filename, 10);

            // Input new file name
            input_file_name();

            // Input new file extension
            write_bottom_screen_message("\n");
            write_bottom_screen_message("Enter file extension: \0");
            cursor_set(cursor_x, cursor_y);

            for (uint8_t i = 0; i < 3; i++) {
                editor_filetype[i] = wait_char();
                putc(editor_filetype[i]);
                cursor_set(cursor_x, cursor_y);
            }

            // Call rename_file() with new file name/ext to overwrite filetable
            rename_file(changed_filename, 10, editor_filename, 10);

            // Call save_file() to update file ext and data on disk
            // TODO: Use actual file size, not hardcoded 1
            if (save_file(editor_filename, editor_filetype, 1, file_address)) {
                write_bottom_screen_message(" Ctrl-R: Return | Ctrl-S: Save | Ctrl-C: Chg name/ext | Ctrl-D: Del line\0");

                cursor_x = save_x;
                cursor_y = save_y;
                cursor_set(cursor_x, cursor_y);

            } else {
                // TODO: handle save errors here
            }

            unsaved = 0;  // Saved file, no unsaved changes now

            continue;
        }

        // Ctrl-D: Delete current line
        if (input_char == F3_BUTTON) {
            // If on last line of file, skip TODO: Put in deleting last line
            if (cursor_y == file_length_lines) continue;

            // Move all lines after this one up 1 line
            file_ptr    -= cursor_x;
            file_offset -= cursor_x;
            save_file_ptr    = file_ptr;
            save_file_offset = file_offset;
            while (file_offset < file_length_bytes - current_line_length + 1) {
                *file_ptr = *(file_ptr + current_line_length);
                file_ptr++;
                file_offset++;
            }
            
            // Blank out all lines from current line to end of file
            save_y = cursor_y;
            while (cursor_y <= file_length_lines) {
                cursor_x = 0;
                for (uint8_t j = 0; j < 79; j++)
                    putc(SPACE);

                cursor_y++;
            }

            file_length_lines--;    // 1 less line in the file now
            file_length_bytes -= current_line_length;

            // Redraw all characters from current line to end of file
            // TODO: Look into refactoring all of this for better performance?
            cursor_x = 0;
            cursor_y = save_y;
            file_ptr = save_file_ptr;

            while (*file_ptr != 0x00) {
                if (*file_ptr == 0x0A) {
                    // Go to next line
                    putc(SPACE); // Print space visually
                    cursor_x = 0;
                    cursor_y++;

                } else
                    putc(*file_ptr);

                file_ptr++;
            }

            // Restore cursor position to start of new line
            cursor_x = 0;
            cursor_y = save_y;
            cursor_set(cursor_x, cursor_y);

            file_ptr    = save_file_ptr;
            file_offset = save_file_offset;

            // Get new current line length
            current_line_length = 0;
            while (*file_ptr != 0x0A && *file_ptr != 0x00) {
                file_ptr++;
                current_line_length++;
            }

            file_ptr = save_file_ptr;   // Restore file data at cursor

            continue;
        }
        
			
        // Backspace or delete
        // TODO: May not be consistent with multiple lines and deleting at different positions
        if (input_char == 0x08 || input_char == BACKSPACE_BUTTON) {
            if (input_char == 0x08 && cursor_x == 0) continue;  // Skip backspace at start of line

            // TODO: Handle newline deletion
            if (*file_ptr == 0x0A) continue;

            // At end of file? Move cursor back
            if (*file_ptr == 0x00 && file_offset != 0) {
                remove_cursor(cursor_x, cursor_y);

                // Go back 1 character
                cursor_x--;
                file_ptr--;
                file_offset--;
                current_line_length--;

                continue;
            }

            // Backspace, move back 1 character/byte
            if (input_char == 0x08) {
                cursor_x--;
                file_ptr--;
                file_offset--;
            }

            // Move all file data ahead of cursor back 1 byte
            for (uint32_t i = 0; i < (file_length_bytes - file_offset); i++)
                file_ptr[i] = file_ptr[i+1];

            file_length_bytes--;    // Deleted a char/byte from file data

            // Rewrite this line
            save_file_ptr = file_ptr;
            save_x = cursor_x;

            file_ptr -= cursor_x;   // Start of line
            cursor_x = 0;
            // Redraw line until end of line or end of file
            while (*file_ptr != 0x0A && *file_ptr != 0x00) {
                putc(*file_ptr);
                file_ptr++;
            }

            putc(SPACE);    // Previous end of line now = space
            current_line_length--;

            // Restore cursor_file position
            cursor_x = save_x;
            file_ptr = save_file_ptr;
            cursor_set(cursor_x, cursor_y);

            unsaved = 1;    // File now has unsaved changes

            continue;
        }

        if (input_char == LEFTARROW) {    // Left arrow key
            // Move 1 byte left (till beginning of line)
            if (cursor_x != 0) {
                remove_cursor(cursor_x, cursor_y);

                file_ptr--;     // Move file data to previous byte
                file_offset--;
                cursor_x--;     // Move cursor to previous character
                cursor_set(cursor_x, cursor_y);
            }
            continue;
        }

        if (input_char == RIGHTARROW) {    // Right arrow key
            // Move 1 byte right (till end of line)
            if ((uint32_t)cursor_x+1 < current_line_length) { 
                remove_cursor(cursor_x, cursor_y);

                file_ptr++;
                file_offset++;
                cursor_x++;
                cursor_set(cursor_x, cursor_y);  // Cursor will be 1 forward from putc above
            }
            continue;
        }

        if (input_char == UPARROW) {      // Up arrow key
            if (cursor_y == 0)  // On 1st line, can't go up
                continue;

            remove_cursor(cursor_x, cursor_y);

			cursor_y--;  // Move cursor 1 line up

            current_line_length = 0;
            file_ptr--;
            file_offset--;

			// Search for end of previous line above current line (newline 0Ah)
            // TODO: End of line could be char position 80, not always a line feed
			while(*file_ptr != 0x0A) {
				file_ptr--;
                file_offset--;
            }

            file_ptr--;             // Move past newline
            file_offset--;
            current_line_length++;  // Include newline as end of current line

			// Search for either start of file (if 1st line) or end of line above
			//  previous line
            // TODO: End of line could be char position 80, not always a line feed
            while (*file_ptr != 0x0A && file_offset != 0) {
				file_ptr--;
                file_offset--;
                current_line_length++;
            }
            
            if (*file_ptr == 0x0A) {
                file_ptr++;         // Move to start of current line
                file_offset++;

            } else if (file_offset == 0) {
                current_line_length++;  // Include 1st byte of file
            }

            // If line is shorter than where cursor is, move cursor to end of shorter line
            if (current_line_length < (uint32_t)cursor_x + 1)  // Cursor is 0-based
                cursor_x = current_line_length - 1;

            file_ptr    += cursor_x;      // offset into line
            file_offset += cursor_x;
            cursor_set(cursor_x, cursor_y);

            continue;
        }

        if (input_char == DOWNARROW) {    // Down arrow key
            if (cursor_y == file_length_lines)  // On last line of file
                continue;
                
            remove_cursor(cursor_x, cursor_y);

            cursor_y++;  // Move cursor down 1 line

            current_line_length = 0;

			// Search file data forwards for a newline (0Ah)
            // TODO: End of line could be char position 80, not always a line feed
			while (*file_ptr != 0x0A) {
				file_ptr++;
                file_offset++;
            }

			// Found end of current line, move past newline
            file_ptr++;
            file_offset++;

            // Now search for end of next line or end of file
            //   File length is 1-based, offset is 0-based
            // TODO: End of line could be char position 80, not always a line feed
			while ((*file_ptr != 0x0A && *file_ptr != 0x00) && (file_offset != file_length_bytes - 1)) {
                file_ptr++;
                file_offset++;
                current_line_length++;
			}

            // Include end of file byte
            current_line_length++;

            // If line is shorter than where cursor is, move cursor to end of shorter line
            if (current_line_length < (uint32_t)cursor_x + 1)  // Cursor is 0-based
                cursor_x = current_line_length - 1;

            // Move to start of current line
            file_ptr    -= current_line_length - 1;    
            file_offset -= current_line_length - 1;    

            file_ptr    += cursor_x;   // Move to cursor position in line
            file_offset += cursor_x;

            cursor_set(cursor_x, cursor_y);

			continue;
        }

        if (input_char == HOMEKEY) {      // Home key
            remove_cursor(cursor_x, cursor_y);

            // Move to beginning of line
            file_ptr    -= cursor_x;   // Move file data to start of line
            file_offset -= cursor_x;

            // Move cursor to start of line
            cursor_x = 0;           
            cursor_set(cursor_x, cursor_y);

            continue;
        }

        if (input_char == ENDKEY) {       // End key
            remove_cursor(cursor_x, cursor_y);

            // Move to end of line
            // Get difference of current_line_length and cursor_x (0-based),
            //   add this difference to cursor_x, and file data
            file_ptr    += ((current_line_length - 1) - cursor_x);
            file_offset += ((current_line_length - 1) - cursor_x);
            cursor_x    += ((current_line_length - 1) - cursor_x);

            cursor_set(cursor_x, cursor_y);
    
            continue;
        }

        current_line_length++;  // Update line length
        file_length_bytes++;    // Update file length
        if (input_char == 0x0D) file_length_lines++;  // Update file length

        for (uint32_t i = (file_length_bytes - file_offset); i > 0; i--)
            file_ptr[i] = file_ptr[i-1];

        if (input_char == 0x0D) input_char = 0x0A;  // Convert CR to LF
        *file_ptr = input_char;

        save_x           = cursor_x;
        save_y           = cursor_y;
        save_file_ptr    = file_ptr;
        save_file_offset = file_offset;

        // Newline, reprint all lines until new end of file
        if (input_char == 0x0A) {
            for (uint8_t i = cursor_x; i < 80; i++)
                putc(SPACE);

            while (cursor_y <= file_length_lines) 
                for (uint8_t i = 0; i < 80; i++) {
                    cursor_x = i;
                    putc(SPACE);
                }

            // Reprint file data from current position
            cursor_x = save_x;
            cursor_y = save_y;
            while (*file_ptr != 0x00) {
                if (*file_ptr == 0x0A) {
                    putc(SPACE);
                    cursor_x = 0;
                    cursor_y++;

                } else putc(*file_ptr);

                file_ptr++;
            }

        } else {
            while (*file_ptr != 0x0A && *file_ptr != 0x00) {
                    putc(*file_ptr);
                    file_ptr++;
            }
        }

        cursor_x    = save_x;
        cursor_y    = save_y;
        file_ptr    = save_file_ptr;
        file_offset = save_file_offset;

        file_ptr++;
        file_offset++;  // Inserted new character, move forward 1

        // If inserted newline, go to start of next line
        if (input_char == 0x0A) {
            cursor_x = 0;
            cursor_y++;

            // Get length of new current line
            current_line_length = 0;
            while ((*file_ptr != 0x0A && *file_ptr != 0x00)) {
                file_ptr++;
                file_offset++;
                current_line_length++;
            }

            // Include end of line or end of file byte
            current_line_length++;

            // Move file data to start of line
            file_ptr    -= current_line_length - 1;
            file_offset -= current_line_length - 1;

        } else cursor_x++;

        cursor_set(cursor_x, cursor_y);    // Move cursor on screen
        unsaved = 1;        // Inserted new character, unsaved changes
    }
}


void input_file_name() {
    for (uint8_t i = 0; i < 10; i++) {
        input_char = wait_char();
        editor_filename[i] = input_char;

        putc(input_char);
        cursor_set(cursor_x, cursor_y);
    }
}

void write_bottom_screen_message(const uint8_t* msg) {
    cursor_x = 0;
    cursor_y = (get_resolution_y() / 16) - 1;

    cursor_set(cursor_x, cursor_y);
    printf(msg);
}