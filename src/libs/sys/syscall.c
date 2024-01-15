#include "../include/syscall.h"

void SYS_print(char* string) {
    asm(
        "movl $0, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%ecx\n"
        "int %1\n"
        :
        : "r"(string), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

void SYS_putc(char character) {
    asm(
        "movl $1, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%ecx\n"
        "movl $1, %%edx\n"
        "int %1\n"
        :
        : "r"((int)character), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

void SYS_clrs() {
    asm(
        "movl $2, %%eax\n"
        "movl $1, %%ebx\n"
        "movl $1, %%edx\n"
        "int %0\n"
        :
        : "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

void SYS_sleep(int milliseconds) {
    asm(
        "movl $3, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%edx\n"
        "int %1\n"
        :
        : "r"(milliseconds), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

char SYS_keyboard_wait_key() {
    char key;
    asm(
        "movl $4, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%ecx\n"
        "movl $0, %%edx\n"
        "int %1\n"
        :
        : "r"(&key), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );

    return key;
}

char SYS_keyboard_get_key() {
    char key;
    asm(
        "movl $5, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%ecx\n"
        "movl $1, %%edx\n"
        "int %1\n"
        :
        : "r"(&key), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );

    return key;
}

// read file

// write file