#pragma once

#include <stdint.h>
#include <stddef.h>


#define SEG(seg)                (seg >> 16)
#define OFF(off)                (off & 0xFFFF)
#define SEGOFF2LINE(segoff)     ((SEG(segoff) << 4) + OFF(segoff))

#define INDEX_FROM_BIT(b) 		(b / 0x20)
#define OFFSET_FROM_BIT(b) 		(b % 0x20)


void* memcpy(void* destination, const void* source, uint16_t num);
void* memcpy_off(void* destination, const void* source, uint16_t offset, uint16_t num);
void* memcpy32(void* dst, const void* src, const uint32_t len);

void* memset(void* pointer, uint8_t value, uint16_t num);
int memcmp(const void* firstPointer, const void* secondPointer, uint16_t num);

void* memmove(void *dest, const void *src, size_t len);
void* memmove32(void* dest, const void* src, size_t len);

void* seg_offset_to_linear(void* address);