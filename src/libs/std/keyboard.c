#include "../include/keyboard.h"

//   _  _________   __  ____   ___    _    ____  ____  
//  | |/ / ____\ \ / / | __ ) / _ \  / \  |  _ \|  _ \ 
//  | ' /|  _|  \ V /  |  _ \| | | |/ _ \ | |_) | | | |
//  | . \| |___  | |   | |_) | |_| / ___ \|  _ <| |_| |
//  |_|\_\_____| |_|   |____/ \___/_/   \_\_| \_\____/


// NULL for stop list for using default \n stop char
void keyboard_read(int mode, uint8_t color, char* stop_list, char* buffer) {
    input_read_stop(mode, color, stop_list, buffer);
    while (stop_list[0] != '\250') { continue; }
}

//====================================================================
//  This function reads keyboard input from user until he press ENTER -
//  that returns string of input
//  EAX - interrupt / buffer
//  EDX - color
//  EBX - mode
void input_read(int mode, uint8_t color, char* buffer) {
    __asm__ volatile(
        "movl $19, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %0, %%edx\n"
        "movl %2, %%ecx\n"
        "int $0x80\n"
        : 
        : "r"((int)color), "r"(mode), "m"(buffer)
        : "eax", "ebx", "ecx", "edx"
    );
}

//====================================================================
//  This function reads keyboard input from user until he press stop character -
//  that returns string of input with stop character of last position
//  EAX - interrupt / buffer
//  EDX - color
//  EBX - mode
//  ECX - stop list
//  ESI - buffer pointer
void input_read_stop(int mode, uint8_t color, char* stop_list, char* buffer) {
    __asm__ volatile (
        "movl $46, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %0, %%edx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%esi\n"
        "int $0x80\n"
        :
        : "m"(color), "m"(mode), "m"(stop_list), "m"(buffer)
        : "eax", "ebx", "edx", "esi"
    );
}
