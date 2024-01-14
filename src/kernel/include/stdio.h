#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "vga_text.h"
#include "string.h"

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void cprintf(uint8_t color, const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void fprintf_unsigned(uint8_t file, unsigned long long number, int radix, int color);

void fputc(char c, uint8_t file, int color);
void cputc(char c, uint8_t color);
void fputs(const char* str, uint8_t file, int color);
void vfprintf(uint8_t file, const char* fmt, va_list args, int color);
void fprintf(uint8_t file, const char* fmt, ...);
void fprint_buffer(uint8_t file, const char* msg, const void* buffer, uint32_t count);

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void cprintf(uint8_t color, const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void debugc(char c);
void debugs(const char* str);
void debugf(const char* fmt, ...);
void debug_buffer(const char* msg, const void* buffer, uint32_t count);

void set_color(int color);
void print_hex_table(const char* data, size_t length);