#include "../include/keyboard.h"
// Shift keyboard converter from https://github.com/cstack/osdev/blob/master/drivers/keyboard.c#L8

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
    '\'', '`', '\252',		                                            /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			                        /* 49 */
    'm', ',', '.', '/',  '\253',				                        /* Right shift */
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
    '\251',	                                                            /* Delete Key */
    0,   0,   0,                        
    0,	                                                                /* F11 Key */
    0,	                                                                /* F12 Key */
    0,	                                                                /* All other keys are undefined */
};

unsigned char shift_alphabet[128] = {
  0, 0, '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', 0, 0,
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', 0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~', '\252', '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', '\253', 0, 0,
  ' ', 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
};


bool lshift_pressed;
bool rshift_pressed;
bool extended_scan_code;
bool key_pressed[128];


char* key_read(int mode, int color) {
    // char* input = malloc(1);
    // input[0] = '\0';
    
    // while (1) {
    //     uint8_t status = i386_inb(0x64);
    //     if (status & 0x1) {
    //         if (status & 0x20) continue;

    //         char character = i386_inb(0x60);
    //         if (character < 0 || character >= 128) continue;

    //         key_pressed[character] = false;
            
    //             char currentCharacter  = get_char();
    //             key_pressed[currentCharacter] = true;

    //             if (key_pressed[LSHIFT] || key_pressed[RSHIFT]) currentCharacter = shift_alphabet[character];
    //             if (currentCharacter == LSHIFT_BUTTON || currentCharacter == RSHIFT_BUTTON) continue;
    //             if (currentCharacter != ENTER_BUTTON) {
    //                 if (currentCharacter == BACKSPACE_BUTTON) {
    //                     if (strlen(input) <= 0) continue;
    //                     input = backspace_string(input);

    //                     if (!is_vesa) {
    //                         VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y());
    //                         VGA_putchr(VGA_cursor_get_x(), VGA_cursor_get_y(), NULL);
    //                     } else VESA_backspace();

    //                     continue;
    //                 }

    //                 if (mode == VISIBLE_KEYBOARD)
    //                     if (color != -1) kcprintf(color, "%c", currentCharacter);
    //                     else kprintf("%c", currentCharacter);

    //                 input = add_char_to_string(input, currentCharacter);
    //             } else break;
            

    //         if (key_pressed[LSHIFT] || key_pressed[RSHIFT]) {
    //             key_pressed[LSHIFT] = false;
    //             key_pressed[RSHIFT] = false;
    //         }
    //     }
    // }

    // return input;
}