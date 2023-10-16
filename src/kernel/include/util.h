#include <stdbool.h> 
#include <stdint.h>

/*
// fixed width integer types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef u32 size_t;
typedef u32 uintptr_t;
typedef float f32;
typedef double f64;

typedef u8 bool;
#define true (1)
#define false (0)

#define NULL (0)
*/

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define __MIN_IMPL(_x, _y, _xn, _yn) __extension__({\
        __typeof__(_x) _xn = (_x);\
        __typeof__(_y) _yn = (_y);\
        (_xn < _yn ? _xn : _yn);\
        })
#define MIN(_x, _y) __MIN_IMPL(_x, _y, CONCAT(__x, __COUNTER__), CONCAT(__y, __COUNTER__))

#define __MAX_IMPL(_x, _y, _xn, _yn) __extension__({\
        __typeof__(_x) _xn = (_x);\
        __typeof__(_y) _yn = (_y);\
        (_xn > _yn ? _xn : _yn);\
        })
#define MAX(_x, _y) __MAX_IMPL(_x, _y, CONCAT(__x, __COUNTER__), CONCAT(__y, __COUNTER__))

#define CLAMP(_x, _mi, _ma) (MAX(_mi, MIN(_x, _ma)))

// returns the highest set bit of x
// i.e. if x == 0xF, HIBIT(x) == 3 (4th index)
// WARNING: currently only works for up to 32-bit types
#define HIBIT(_x) (31 - __builtin_clz((_x)))

// returns the lowest set bit of x
#define LOBIT(_x)\
    __extension__({ __typeof__(_x) __x = (_x); HIBIT(__x & -__x); })

// returns _v with _n-th bit = _x
#define BIT_SET(_v, _n, _x) __extension__({\
        __typeof__(_v) __v = (_v);\
        (__v ^ ((-(_x) ^ __v) & (1 << (_n))));\
        })

#define PACKED __attribute__((packed))

#ifndef asm
#define asm __asm__ volatile
// #endif

#define CLI() asm ("cli")
#define STI() asm ("sti")

static inline uint16_t inportw(uint16_t port) {
    uint16_t r;
    asm("inw %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outportw(uint16_t port, uint16_t data) {
    asm("outw %1, %0" : : "dN" (port), "a" (data));
}

static inline uint8_t inportb(uint16_t port) {
    uint8_t r;
    asm("inb %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outportb(uint16_t port, uint8_t data) {
    asm("outb %1, %0" : : "dN" (port), "a" (data));
}

static inline void cmos_select_register(uint8_t registerNumber) {
    bool NMI_DISABLE = 0;
    outportb(0x70, (NMI_DISABLE << 7) | registerNumber);
    // TODO: Do I need some type of delay here?
}

static inline uint8_t cmos_read() {
    return inportb(0x71);
}

static inline char* drive_get_description(uint8_t driveDesc) {
    switch (driveDesc) {
        case 0:
            return "NOT FOUND";
        case 1:
            return "360 KB 5.25 DRIVE";
        case 2:
            return "1.2 MB 5.25 DRIVE";
        case 3:
            return "720 KB 3.5 DRIVE";
        case 4:
            return "1.44 MB 3.5 DRIVE";
        case 5:
            return "2.88 MB 3.5 DRIVE";
        default:
            return "DRIVE TYPE UNKNOWN";
    }
}

#endif