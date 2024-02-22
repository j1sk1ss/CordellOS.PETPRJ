#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "vga_text.h"
#include "vesa_text.h"
#include "gfx.h"


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


extern bool is_vesa;


void kclrscr();
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