#include <stdint.h>

#include "fatlib.h"

#define SYSCALL_INTERRUPT 0x80

void SYS_puts(char* string);
void SYS_putc(char character);
void SYS_cputc(uint8_t color, char character);

void SYS_clrs();
void SYS_scrclr(uint8_t color);

void SYS_sleep(int milliseconds);

char SYS_keyboard_wait_key();
char SYS_keyboard_get_key();

void SYS_get_datetime(short* data);

void* SYS_malloc(uint32_t size);
void SYS_free(void* ptr);

char* SYS_fread(const char* path);
void SYS_fwrite(const char* path, const char* data);

struct UFATDirectory* SYS_opendir(const char* path);