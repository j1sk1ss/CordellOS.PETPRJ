#pragma once

#include <stdint.h>

void __attribute__((cdecl)) i686_outb(uint16_t port, uint8_t data);
uint8_t __attribute__((cdecl)) i686_inb(uint16_t port);
uint8_t __attribute__((cdecl)) i686_enableInterrupts();
uint8_t __attribute__((cdecl)) i686_disableInterrupts();

//
//  Wait for inpet char / string
//
char __attribute__((cdecl)) x86_inputWait();

void i686_io_wait();