#include "io/stdio.h"
#include "io/string.h"

#include "Keyboard.h"

#include "memory/memory.h"
#include "memory/allocator/malloc.h"

#include <arch/i686/io.h>

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
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
    0,	                                        /* Alt */
    ' ',	                                    /* Space bar */
    0,	                                        /* Caps lock */
    0,	                                        /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	                                        /* < ... F10 */
    0,	                                        /* 69 - Num lock*/
    0,	                                        /* Scroll Lock */
    0,	                                        /* Home key */
    0,	                                        /* Up Arrow */
    0,	                                        /* Page Up */
    '-',
    0,	                                        /* Left Arrow */
    0,
    0,	                                        /* Right Arrow */
    '+',
    0,	                                        /* 79 - End key*/
    0,	                                        /* Down Arrow */
    0,	                                        /* Page Down */
    0,	                                        /* Insert Key */
    0,	                                        /* Delete Key */
    0,   0,   0,
    0,	                                        /* F11 Key */
    0,	                                        /* F12 Key */
    0,	                                        /* All other keys are undefined */
};


char* keyboard_read(int visibility) {
    char* input       = NULL;  // Start with an empty string
    size_t input_size = 0;

    while (1) {
        if (i686_inb(0x64) & 0x1) {
            char character = i686_inb(0x60);

            if (!(character & 0x80)) {
                char currentCharacter = alphabet[character];
                if (currentCharacter != '\n') {
                    if (visibility == 1)
                        printf("%c", currentCharacter);

                    // Allocate memory to accommodate the new character
                    char* buffer = (char*)malloc(++input_size + 1);
                    memset(buffer, 0, sizeof(buffer));
                    if (buffer == NULL)
                        return NULL;

                    strcpy(buffer, input);

                    if (buffer == NULL) {
                        printf("\nMemory allocation failed\n");
                        free(buffer);

                        return NULL;
                    }

                    input = buffer;

                    input[input_size - 1] = currentCharacter;     // Set last character
                    input[input_size]     = '\0';                 // Null-terminate the string
                } 
                else break;
            }
        }
    }

    return input;
}