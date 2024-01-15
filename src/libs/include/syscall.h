#include "x86.h"

#define SYSCALL_INTERRUPT 0x80

void SYS_print(char* string);
void SYS_putc(char character);
void SYS_clrs();
void SYS_sleep(int milliseconds);

char SYS_keyboard_wait_key();
char SYS_keyboard_get_key();