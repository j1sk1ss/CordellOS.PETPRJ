#include "../include/memory.h"

void* memcpy(void* destination, const void* source, uint16_t num) {
    uint8_t* u8Dst = (uint8_t*)destination;
    const uint8_t* u8Src = (const uint8_t*)source;

    for (uint16_t i = 0; i < num; i++)
        u8Dst[i] = u8Src[i];

    return destination;
}

void* memcpy_off(void* destination, const void* source, uint16_t offset, uint16_t num) {
    uint8_t* u8Dst = (uint8_t*)destination;
    const uint8_t* u8Src = (const uint8_t*)source;

    for (uint16_t i = offset; i < offset + num; i++)
        u8Dst[i] = u8Src[i];

    return destination;
}

void* memcpy32(void* dst, const void* src, const uint32_t len) {
    uint32_t i = 0;

    for (i = 0; i < len / 4; i++)
        ((uint32_t *)dst)[i] = ((uint32_t *)src)[i];

    i *= 4;
    for (uint32_t j = 0; j < len % 4; i++, j++)
        ((uint8_t *)dst)[i] = ((uint8_t *)src)[i];

    return dst;
}

void* memset(void* pointer, uint8_t value, uint16_t num) {
    uint8_t* u8Ptr = (uint8_t*)pointer;

    for (uint16_t i = 0; i < num; i++)
        u8Ptr[i] = value;

    return pointer;
}

int memcmp(const void* firstPointer, const void* secondPointer, uint16_t num) {
    const uint8_t* u8Ptr1 = (const uint8_t *)firstPointer;
    const uint8_t* u8Ptr2 = (const uint8_t *)secondPointer;

    for (uint16_t i = 0; i < num; i++)
        if (u8Ptr1[i] != u8Ptr2[i])
            return 1;

    return 0;
}

void* seg_offset_to_linear(void* address) {
    uint32_t offset = (uint32_t)(address) & 0xFFFF;
    uint32_t segment = (uint32_t)(address) >> 16;

    return (void*)(segment * 16 + offset);
}

void* memmove(void* dest, const void* src, size_t len) {
    char* d = dest;
    const char* s = src;
    if (d < s)
        while (len--)
            *d++ = *s++;
    else {
      const char* lasts = s + (len - 1);
      char* lastd = d + (len - 1);
        while (len--)
            *lastd-- = *lasts--;
    }

    return dest;
}

void* memmove32(void* dest, const void* src, size_t len) {
    uint32_t* d = (uint32_t*)dest;
    const uint32_t* s = (const uint32_t*)src;

    if (d < s) {
        while (len >= 4) {
            *d++ = *s++;
            len -= 4;
        }
    } else {
        d += len / 4;
        s += len / 4;
        while (len >= 4) {
            *--d = *--s;
            len -= 4;
        }
    }

    uint8_t* d_byte = (uint8_t*)d;
    const uint8_t* s_byte = (const uint8_t*)s;
    while (len--) *d_byte++ = *s_byte++;
    
    return dest;
}