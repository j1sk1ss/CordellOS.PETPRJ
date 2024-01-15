#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "vga_text.h"
#include "string.h"

void clrscr();
void kputc(char c);
void kputs(const char* str);
void kprintf(const char* fmt, ...);
void kcprintf(uint8_t color, const char* fmt, ...);
void kprint_buffer(const char* msg, const void* buffer, uint32_t count);

void kfprintf_unsigned(uint8_t file, unsigned long long number, int radix, int color);

void kfputc(char c, uint8_t file, int color);
void kcputc(char c, uint8_t color);
void kfputs(const char* str, uint8_t file, int color);
void kvfprintf(uint8_t file, const char* fmt, va_list args, int color);
void kfprintf(uint8_t file, const char* fmt, ...);
void kfprint_buffer(uint8_t file, const char* msg, const void* buffer, uint32_t count);

void kputc(char c);
void kputs(const char* str);
void kprintf(const char* fmt, ...);
void kcprintf(uint8_t color, const char* fmt, ...);
void kprint_buffer(const char* msg, const void* buffer, uint32_t count);

void kset_color(int color);
void kprint_hex_table(const char* data, size_t length);