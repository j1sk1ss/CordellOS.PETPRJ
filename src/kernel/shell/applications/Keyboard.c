#include "../include/Keyboard.h"

#include "../../../libs/core/shared/include/allocator.h"

#include "../../include/stdio.h"
#include "../../include/string.h"
#include "../../include/memory.h"
#include "../../include/io.h"
#include "../../include/vga_text.h"

/* 
*  KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! 
*/

unsigned char alphabet[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	                    /* 9 */
    '9', '0', '-', '=', '\b',	                                        /* Backspace */
    '\t',			                                                    /* Tab */
    'q', 'w', 'e', 'r',	                                                /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	                    /* Enter key */
    0,			                                                        /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	                /* 39 */
    '\'', '`',   0,		                                                /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			                        /* 49 */
    'm', ',', '.', '/',   0,				                            /* Right shift */
    '*',
    0,	                                                                /* Alt */
    ' ',	                                                            /* Space bar */
    '\5',	                                                            /* Caps lock */
    '\6',	                                                            /* 59 - F1 key ... > */
    '\7',   '\255',   0,   0,   0,   0,   0,   0,                       
    0,	                                                                /* < ... F10 */
    0,	                                                                /* 69 - Num lock*/
    0,	                                                                /* Scroll Lock */
    0,	                                                                /* Home key */
    '\4',	                                                            /* Up Arrow */
    0,	                                                                /* Page Up */
    '-',                        
    '\1',	                                                            /* Left Arrow */
    0,                      
    '\2',	                                                            /* Right Arrow */
    '+',                        
    0,	                                                                /* 79 - End key*/
    '\3',	                                                            /* Down Arrow */
    0,	                                                                /* Page Down */
    0,	                                                                /* Insert Key */
    0,	                                                                /* Delete Key */
    0,   0,   0,                        
    0,	                                                                /* F11 Key */
    0,	                                                                /* F12 Key */
    0,	                                                                /* All other keys are undefined */
};



/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
////
////
////    SHELL KEYBOARD
////

        char* keyboard_read(int mode, int color) {
            char* input = (char*)malloc(sizeof(char));  // Start with an empty string
            memset(input, 0, sizeof(input));

            size_t input_size = 0;

            while (1) {
                if (i686_inb(0x64) & 0x1) {
                    char character = i686_inb(0x60);

                    if (!(character & 0x80)) {
                        char currentCharacter = alphabet[character];
                        if (currentCharacter != '\n') {
                            if (currentCharacter == '\b') {
                                if (strlen(input) > 0 && input_size > 0)
                                    backspace_string(&input, --input_size);
                                
                                continue;
                            }

                            if (mode == VISIBLE_KEYBOARD)
                                cprintf(color, "%c", currentCharacter);

                            add_char_to_string(&input, ++input_size, currentCharacter);
                        }
                        else break;
                    }
                }
            }

            return input;
        }

////
////    SHELL KEYBOARD
////
////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
////
////    EDITOR KEYBOARD
////
////

        char* keyboard_edit(char* previous_data, int color) {
            char* input = (char*)malloc(strlen(previous_data));  // Start with an empty string
            memset(input, 0, sizeof(input));
            strcpy(input, previous_data);

            cprintf(color, "%s", input);

            size_t input_size = strlen(input);

            while (1) {
                if (i686_inb(0x64) & 0x1) {
                    char character = i686_inb(0x60);

                    if (!(character & 0x80)) {
                        char currentCharacter = alphabet[character];
                        if (currentCharacter != '\5') {

                            if (currentCharacter == '\1') { // Left
                                VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y()); 
                                continue;
                            }
                            else if (currentCharacter == '\2') { // Right
                                if (VGA_getchr(VGA_cursor_get_x() + 1, VGA_cursor_get_y()) != NULL)
                                    VGA_setcursor(VGA_cursor_get_x() + 1, VGA_cursor_get_y()); 

                                continue;
                            } 
                            else if (currentCharacter == '\3') { // Down
                                if (VGA_getchr(VGA_cursor_get_x(), VGA_cursor_get_y() + 1) != NULL)
                                    VGA_setcursor(VGA_cursor_get_x(), VGA_cursor_get_y() + 1); 

                                continue;
                            }
                            else if (currentCharacter == '\4') { // Up
                                if (VGA_getchr(VGA_cursor_get_x(), VGA_cursor_get_y() - 1) != NULL)
                                    VGA_setcursor(VGA_cursor_get_x(), VGA_cursor_get_y() - 1); 

                                continue;
                            }
                            else if (currentCharacter == '\b') {
                                if (strlen(input) > 0 && input_size > 0) 
                                    backspace_string(&input, --input_size);
                                
                                continue;
                            }
                            else if (currentCharacter == '\n') {
                                cprintf(color, "\n");                        
                                add_char_to_string(&input, ++input_size, '\n');
                            }
                            else {
                                cprintf(color, "%c", currentCharacter);
                                add_char_to_string(&input, ++input_size, currentCharacter);
                            }
                        }
                        else break;
                    }
                }
            }

            return input;
        }

////
////
////    EDITOR KEYBOARD
////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
////
////    NAVIGATION KEYBOARD
////
////

        char keyboard_navigation() {
            while (1) 
                if (i686_inb(0x64) & 0x1) 
                    return alphabet[i686_inb(0x60)];
        }

////
////    NAVIGATION KEYBOARD
////
////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////