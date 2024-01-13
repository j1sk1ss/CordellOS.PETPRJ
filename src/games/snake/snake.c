#include "libs/include/date_time.h"
#include "libs/include/vga_text.h"
#include "libs/include/vfs.h"
#include "libs/include/stdio.h"

int* main(void) {
	while(1) {
		print_char('H');
	}

	return datetime_day;
}

#define VIDEO_MEMORY 0xB8000
#define ATTRIBUTE_BYTE 0x07 // Light gray on black (color attribute)

void print_char(const char str) {
    volatile char* video_memory = (volatile char*)VIDEO_MEMORY;
    video_memory[0] = str;
}