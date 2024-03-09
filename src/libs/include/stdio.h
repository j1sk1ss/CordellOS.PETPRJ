#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "string.h"


#define SYSCALL_INTERRUPT 0x80


//=======================================
//  COLORS

    // General Formatting
    #define GEN_FORMAT_RESET        0x00
    #define GEN_FORMAT_BRIGHT       0x01
    #define GEN_FORMAT_DIM          0x02
    #define GEN_FORMAT_UNDERSCORE   0x03
    #define GEN_FORMAT_BLINK        0x04
    #define GEN_FORMAT_REVERSE      0x05
    #define GEN_FORMAT_HIDDEN       0x06

    // Foreground Colors
    #define FOREGROUND_BLACK        0x00
    #define FOREGROUND_BLUE         0x01
    #define FOREGROUND_GREEN        0x02
    #define FOREGROUND_AQUA         0x03
    #define FOREGROUND_RED          0x04
    #define FOREGROUND_PURPLE       0x05
    #define FOREGROUND_YELLOW       0x06
    #define FOREGROUND_WHITE        0x07
    #define FOREGROUND_GREY         0x08
    #define FOREGROUND_LIGHT_BLUE   0x09
    #define FOREGROUND_LIGHT_GREEN  0x0A
    #define FOREGROUND_LIGHT_AQUA   0x0B
    #define FOREGROUND_LIGHT_RED    0x0C
    #define FOREGROUND_LIGHT_PURPLE 0x0D
    #define FOREGROUND_LIGHT_YELLOW 0x0E
    #define FOREGROUND_BRIGHT_WHITE 0x0F

    // Background Colors
    #define BACKGROUND_BLACK        0x00
    #define BACKGROUND_BLUE         0x10
    #define BACKGROUND_GREEN        0x20
    #define BACKGROUND_AQUA         0x30
    #define BACKGROUND_RED          0x40
    #define BACKGROUND_PURPLE       0x50
    #define BACKGROUND_YELLOW       0x60
    #define BACKGROUND_WHITE        0x70

//  COLORS
//=======================================
//  MODS

    #define HIDDEN_KEYBOARD         0
    #define VISIBLE_KEYBOARD        1

//  MODS
//=======================================


#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


void directly_putclr(int x, int y, uint8_t color);
void directly_putc(int x, int y, char character);
char directly_getchar(int x, int y);
void cursor_set(int x, int y);
void cursor_set32(uint32_t x, uint32_t y);
void cursor_get(int* result);

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void cprintf(uint32_t color, const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void fprintf_unsigned(uint8_t file, unsigned long long number, int radix, int color);

void fputc(char c, uint32_t file, int color);
void cputc(char c, uint32_t color);
void fputs(const char* str, uint8_t file, int color);
void vfprintf(uint32_t file, const char* fmt, va_list args, int color);
void fprintf(uint8_t file, const char* fmt, ...);
void fprint_buffer(uint8_t file, const char* msg, const void* buffer, uint32_t count);

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void cprintf(uint32_t color, const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void set_color(int color);
void print_hex_table(const char* data, size_t length);