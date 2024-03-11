#include "../../include/keyboard.h"

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
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	                    /* 9 */
    '(', ')', '_', '+', '\b',	                                        /* Backspace */
    '\t',			                                                    /* Tab */
    'Q', 'W', 'E', 'R',	                                                /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	                    /* Enter key */
    0,			                                                        /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	                /* 39 */
    '"', '~', '\252',		                                            /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',			                        /* 49 */
    'M', '<', '>', '?',  '\253',				                        /* Right shift */
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

uint8_t* char_buffer = NULL;
uint8_t* stop_buffer = NULL;
int mode  = HIDDEN_KEYBOARD;
int color = WHITE;
int pos   = 0;

bool lshift_pressed;
bool rshift_pressed;
bool extended_scan_code;
bool key_pressed[128];


void i386_init_keyboard() {
    i386_outb(0x64, 0xFF);
    uint8_t status = i386_inb(0x64);
    status = i386_inb(0x64);

    if (status & (1 << 0)) kprintf("Output buffer full.\n");
    else kprintf("Output buffer empty.\n");
    if (status & (1 << 1)) kprintf("Input buffer full.\n");
    else kprintf("Input buffer empty.\n");
    if (status & (1 << 2)) kprintf("System flag set.\n");
    else kprintf("System flag unset.\n");
    if (status & (1 << 3)) kprintf("Command/Data -> PS/2 device.\n");
    else kprintf("Command/Data -> PS/2 controller.\n");
    if (status & (1 << 6)) kprintf("Timeout error.\n");
    else kprintf("No timeout error.\n");
    if (status & (1 << 7)) kprintf("Parity error.\n");
    else kprintf("No parity error.\n");
    
    i386_outb(0x64, 0xAA);
    uint8_t result = i386_inb(0x60);
    if (result == 0x55) kprintf("PS/2 controller test passed.\n");
    else if (result == 0xFC) kprintf("PS/2 controller test failed.\n");
    else {
        kprintf("PS/2 controller responded to test with unknown code %x\n", result);
        kprintf("Trying to continue.\n");
    }

    i386_outb(0x64, 0x20);
    result = i386_inb(0x60);
    kprintf("PS/2 config byte: %x\n", result);

    i386_irq_registerHandler(1, i386_keyboard_handler);
}

int key_press() {
    if (i386_inb(0x64) & 0x1) return 1;
    return 0;
}

char get_character(char character) {
    return alphabet[character];
}

//==================================================================================
//   _  _________   ______   ___    _    ____  ____  
//  | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \ 
//  | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | |
//  | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| |
//  |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/ 

        void enable_keyboard(uint8_t* buffer, int keyboard_mode, int keyboard_color, uint8_t* stop) {
            char_buffer = buffer;
            stop_buffer = stop;            
            mode  = keyboard_mode;
            color = keyboard_color;
            pos   = 0;
        }

        void i386_keyboard_handler(Registers* regs) {
            char character = i386_inb(0x60);
            if (character < 0 || character >= 128) return;
            if (char_buffer == NULL || stop_buffer == NULL) return;

            char* input = char_buffer;
            input[pos] = '\0';

            key_pressed[character] = false;
            if (!(character & 0x80)) {
                key_pressed[character] = true;
                char currentCharacter = alphabet[character];
                int chr_pos = -1;
                while (stop_buffer[++chr_pos] != '\0') 
                    if (stop_buffer[chr_pos] == currentCharacter) {
                        input[max(0, strlen(input))] = stop_buffer[chr_pos];
                        stop_buffer[0] = '\250';

                        char_buffer = NULL;
                        stop_buffer = NULL;
                        mode  = HIDDEN_KEYBOARD;
                        color = -1;
                        pos   = 0;

                        return;
                    }

                if (key_pressed[LSHIFT] || key_pressed[RSHIFT]) currentCharacter = shift_alphabet[character];
                if (currentCharacter == LSHIFT_BUTTON || currentCharacter == RSHIFT_BUTTON) return;
                if (currentCharacter == BACKSPACE_BUTTON) {
                    if (strlen(input) <= 0) return;
                    input[pos--] = '\0';

                    if (!is_vesa) {
                        VGA_setcursor(VGA_cursor_get_x() - 1, VGA_cursor_get_y());
                        VGA_putchr(VGA_cursor_get_x(), VGA_cursor_get_y(), NULL);
                    } else VESA_backspace();

                    return;
                }

                if (mode == VISIBLE_KEYBOARD)
                    if (color != -1) kcprintf(color, "%c", currentCharacter);
                    else kprintf("%c", currentCharacter);

                input[pos++] = currentCharacter;
            }

            if (key_pressed[LSHIFT] || key_pressed[RSHIFT]) {
                key_pressed[LSHIFT] = false;
                key_pressed[RSHIFT] = false;
            }
        }

//==================================================================================