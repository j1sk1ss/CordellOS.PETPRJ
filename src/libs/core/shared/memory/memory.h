#pragma once

#include <stdint.h>

#define SEG(seg)         (seg >> 16)
#define OFF(off)         (off & 0xFFFF)
#define SEGOFF2LINE(segoff)    ((SEG(segoff) << 4) + OFF(segoff))

void* memcpy(void* dst, const void* src, uint16_t num);
void* memset(void* ptr, int value, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);

void* seg_offset_to_linear(void* address);