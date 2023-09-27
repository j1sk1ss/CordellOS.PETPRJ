#include "../include/string.h"
#include "../include/stdio.h"

#include "../Keyboard.h"

char* cordell_nano_editor(char* previous_data) {
    clrscr();
    printf("You are editing file. Use CAPSLOCK to exit.\r\n\r\n");
    printf("%s", previous_data);

    char* text = keyboard_edit(previous_data);
    return text;    
}