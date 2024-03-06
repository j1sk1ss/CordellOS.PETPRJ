#include "../include/sysinfo.h"


void print_malloc_map() {
    __asm__ volatile (
        "movl $48, %%eax\n"
        "int $0x80\n"
        :
        :
        : "eax", "ebx", "ecx", "edx"
    );
}

void print_page_map(char arg) {
    __asm__ volatile (
        "movl $49, %%eax\n"
        "movl %0, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((int)arg)
        : "eax", "ebx", "ecx", "edx"
    );
}