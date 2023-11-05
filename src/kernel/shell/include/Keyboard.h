#include "../../include/debug.h"

#include <stddef.h>
#include <stdint.h>

#define HIDDEN_KEYBOARD         0
#define VISIBLE_KEYBOARD        1

char* keyboard_read(int mode);
char* keyboard_edit(char* previous_data);

void __attribute__((cdecl)) x86_init_keyboard();