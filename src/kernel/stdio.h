#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <hal/vfs.h>

void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void fputc(char c, fileDescriptorId file);
void fputs(const char* str, fileDescriptorId file);
void vfprintf(fileDescriptorId file, const char* fmt, va_list args);
void fprintf(fileDescriptorId file, const char* fmt, ...);
void fprint_buffer(fileDescriptorId file, const char* msg, const void* buffer, uint32_t count);

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void debugc(char c);
void debugs(const char* str);
void debugf(const char* fmt, ...);
void debug_buffer(const char* msg, const void* buffer, uint32_t count);