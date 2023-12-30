#include "../include/x86.h"

uint16_t x86_inw(uint16_t port) {
    uint16_t r;
    asm("inw %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

void x86_outw(uint16_t port, uint16_t data) {
    asm("outw %1, %0" : : "dN" (port), "a" (data));
}

uint8_t x86_inb(uint16_t port) {
    uint8_t r;
    asm("inb %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

void x86_outb(uint16_t port, uint8_t data) {
    asm("outb %1, %0" : : "dN" (port), "a" (data));
}