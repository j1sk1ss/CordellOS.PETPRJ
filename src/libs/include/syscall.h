#pragma once

#include <stdint.h>

#include "fatlib.h"

#define SYSCALL_INTERRUPT 0x80

void SYS_puts(char* string);
void SYS_putc(char character);
void SYS_cputc(uint8_t color, char character);
void SYS_clrs();
void SYS_scrclr(uint8_t color);
char SYS_scrchar(int x, int y);
int* SYS_get_cursor();
void SYS_set_cursor(int x, int y);
void SYS_set_scrchar(int x, int y, char character);
void SYS_set_scrcolor(int x, int y, uint8_t color);

void SYS_sleep(int milliseconds);
char SYS_keyboard_wait_key();
char SYS_keyboard_get_key();
char* SYS_keyboard_read(int mode, uint8_t color);
void SYS_get_datetime(short* data);

void* SYS_malloc(uint32_t size);
void SYS_free(void* ptr);

int SYS_cexists(const char* path);
void SYS_rmcontent(const char* path, const char* name);
void SYS_chgcontent(const char* path, struct udirectory_entry* meta);

char* SYS_fread(const char* path);
void SYS_fwrite(const char* path, const char* data);
void SYS_mkfile(const char* path, const char* name);
int SYS_fexec(char* path, int args, char** argv);

struct UFATDirectory* SYS_opendir(const char* path);
void SYS_mkdir(const char* path, const char* name);