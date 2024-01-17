#pragma once

#include <stdint.h>

#define ASMCALL __attribute__((cdecl))
#define asm __asm__ volatile

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);
uint8_t __attribute__((cdecl)) i686_enableInterrupts();
uint8_t __attribute__((cdecl)) i686_disableInterrupts();

char __attribute__((cdecl)) x86_inputWait();

void i686_io_wait();