#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ASMCALL __attribute__((cdecl))

//
//  Output data to address
//
void ASMCALL x86_outb(uint16_t port, uint8_t data);

//
//  Output data to address
//
void ASMCALL x86_outw(uint16_t port, short* data);

//
//  Input data from address
//
uint8_t ASMCALL x86_inb(uint16_t port);

//
//  Input data from address
//
uint8_t ASMCALL x86_inw(uint16_t port);