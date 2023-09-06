#pragma once

#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);


#define BOCHS_BREAKPOINT    __asm("xchgw %bx, %bx")