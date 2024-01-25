#include "../include/keyboard.h"


//   _  _________   __  ____   ___    _    ____  ____  
//  | |/ / ____\ \ / / | __ ) / _ \  / \  |  _ \|  _ \ 
//  | ' /|  _|  \ V /  |  _ \| | | |/ _ \ | |_) | | | |
//  | . \| |___  | |   | |_) | |_| / ___ \|  _ <| |_| |
//  |_|\_\_____| |_|   |____/ \___/_/   \_\_| \_\____/


/* 
*  KBDUS means US keyboard Layout. This is a scancode table
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
    '\7',   '\255',   '\254',   0,   0,   0,   0,   0,                       
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

int key_press() {
    if (x86_inb(0x64) & 0x1)
        return 1;

    return 0;
}

char get_character(char character) {
    return alphabet[character];
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//   ____  _   _ _____ _     _       _  _________   ______   ___    _    ____  ____  
//  / ___|| | | | ____| |   | |     | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \ 
//  \___ \| |_| |  _| | |   | |     | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | |
//   ___) |  _  | |___| |___| |___  | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| |
//  |____/|_| |_|_____|_____|_____| |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/ 

        char* keyboard_read(int mode, int color) {
            char* input = (char*)malloc(sizeof(char));  // Start with an empty string
            size_t input_size = 0;
            
            while (1) {
                if (x86_inb(0x64) & 0x1) {
                    char character = x86_inb(0x60);

                    if (!(character & 0x80)) {
                        char currentCharacter = alphabet[character];
                        if (currentCharacter != ENTER_BUTTON) {
                            if (currentCharacter == BACKSPACE_BUTTON) {
                                if (input_size > 0) {
                                    backspace_string(&input, --input_size);
                                    VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y());
                                    VGA_putchr(VGA_cursor_get_x(), VGA_cursor_get_y(), NULL);
                                }

                                continue;
                            }

                            if (mode == VISIBLE_KEYBOARD)
                                if (color != -1) kcprintf(color, "%c", currentCharacter);
                                else kprintf("%c", currentCharacter);

                            add_char_to_string(&input, ++input_size, currentCharacter);
                        }
                        else break;
                    }
                }
            }

            return input;
        }

////
////    
////
////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//   _   _    ___     _____ ____    _  _____ ___ ___  _   _   _  _________   ______   ___    _    ____  ____  
//  | \ | |  / \ \   / /_ _/ ___|  / \|_   _|_ _/ _ \| \ | | | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \ 
//  |  \| | / _ \ \ / / | | |  _  / _ \ | |  | | | | |  \| | | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | |
//  | |\  |/ ___ \ V /  | | |_| |/ ___ \| |  | | |_| | |\  | | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| |
//  |_| \_/_/   \_\_/  |___\____/_/   \_\_| |___\___/|_| \_| |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/ 

        char keyboard_navigation() {
            while (1) 
                if (x86_inb(0x64) & 0x1) {
                    char character = x86_inb(0x60);
                    if (!(character & 0x80)) {
                        return alphabet[character];
                    }
                }
        }

        void keyboard_wait(char symbol) {
            while (1) {
                char user_action = keyboard_navigation();
                if (user_action == symbol)
                    break;
            }
        }

////
////    
////
////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////