#include <stddef.h>
#include <stdint.h>

char map_scan_code_to_char(uint8_t scan_code);
char handle_key_event(uint8_t key_code);
void keyboard_read(char* string);

void __attribute__((cdecl)) x86_init_keyboard();