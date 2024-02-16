#include "../include/graphics.h"


//====================================================================
// Function directly in screen put pixel by coordinates
// EBX - x
// ECX - y
// EDX - pixel data
void put_pixel(int x, int y, uint32_t color) {
    // asm (
    //     "mov $28, %%eax\n"
    //     "mov %0, %%ebx\n"
    //     "mov %1, %%ecx\n"
    //     "mov %2, %%edx\n"
    //     "int $0x80\n"
    //     :
    //     : "r"(x), "r"(y), "r"((int)color)
    //     : "eax", "ebx", "ecx", "edx"
    // );
}

//====================================================================
// Function get pixel from framebuffer by coordinates
// EBX - x
// ECX - y
uint32_t get_pixel(int x, int y) {
    uint32_t result;
    // asm (
    //     "mov $29, %%eax\n"
    //     "mov %1, %%ebx\n"
    //     "mov %2, %%ecx\n"
    //     "int $0x80\n"
    //     "mov %%eax, %0\n"
    //     : "=r"(result)
    //     : "r"(x), "r"(y)
    //     : "eax", "ebx", "ecx"
    // );

    return result;
}