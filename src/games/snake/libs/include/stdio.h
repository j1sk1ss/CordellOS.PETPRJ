#pragma once

#include <stdint.h>
#include <stdarg.h>

#include "vfs.h"
#include "string.h"

void clrscr();

void fprintf_unsigned(fileDescriptorId file, unsigned long long number, int radix, int color);

void fputc(char c, fileDescriptorId file, int color);
void cputc(char c, uint8_t color);
void fputs(const char* str, fileDescriptorId file, int color);
int vfprintf(fileDescriptorId file, const char* fmt, va_list args, int color);
void fprintf(fileDescriptorId file, const char* fmt, ...);
void fprint_buffer(fileDescriptorId file, const char* msg, const void* buffer, uint32_t count);

void putc(char c);
void puts(const char* str);
int printf(const char* fmt, ...);
void cprintf(uint8_t color, const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void debugc(char c);
void debugs(const char* str);
void debugf(const char* fmt, ...);
void debug_buffer(const char* msg, const void* buffer, uint32_t count);

void set_color(int color);