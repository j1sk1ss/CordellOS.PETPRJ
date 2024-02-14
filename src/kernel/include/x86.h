#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ASMCALL __attribute__((cdecl))
#define asm __asm__ volatile

#define kernel_panic(data) kprintf(data);i386_panic();

uint16_t i386_inw(uint16_t port);
void i386_outw(uint16_t port, uint16_t data);
uint8_t i386_inb(uint16_t port);
void i386_outb(uint16_t port, uint8_t data);

uint8_t __attribute__((cdecl)) i386_enableInterrupts();
uint8_t __attribute__((cdecl)) i386_disableInterrupts();

char __attribute__((cdecl)) i386_inputWait();

void i386_io_wait();
void i386_reboot();