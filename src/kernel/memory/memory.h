#pragma once

#include <stdint.h>

#define SEG(seg)                (seg >> 16)
#define OFF(off)                (off & 0xFFFF)
#define SEGOFF2LINE(segoff)     ((SEG(segoff) << 4) + OFF(segoff))

void* memcpy(void* destination, const void* source, uint16_t num);
void* memset(void* pointer, int value, uint16_t num);
int memcmp(const void* firstPointer, const void* secondPointer, uint16_t num);

void* seg_offset_to_linear(void* address);