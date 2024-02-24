#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ASMCALL __attribute__((cdecl))
#define asm __asm__ volatile
#define UNUSED_PORT 0x80
#define kernel_panic(data) kprintf(data);i386_panic();

uint32_t i386_inl(uint16_t port);
void i386_outl(uint16_t port, uint32_t data);
uint16_t i386_inw(uint16_t port);
void i386_outw(uint16_t port, uint16_t data);
uint8_t i386_inb(uint16_t port);
void i386_outb(uint16_t port, uint8_t data);

uint32_t mem_inl(uint32_t addr);
void mem_outl(uint32_t addr, uint32_t value);
uint16_t mem_inw(uint32_t addr);
void mem_outw(uint32_t addr, uint16_t value);
uint8_t mem_inb(uint32_t addr);
void mem_outb(uint32_t addr, uint8_t value);

uint8_t __attribute__((cdecl)) i386_enableInterrupts();
uint8_t __attribute__((cdecl)) i386_disableInterrupts();

char __attribute__((cdecl)) i386_inputWait();

void i386_io_wait();
void i386_reboot();