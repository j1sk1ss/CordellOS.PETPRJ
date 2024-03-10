#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <fslib.h>
#include <memory.h>
#include <keyboard.h>
#include <font.h>


#define SCREEN_HEIGHT   29
#define CHARS_ON_LINE   60
#define BYTES_ON_LINE   CHARS_ON_LINE * CHARLEN
#define SCREEN_SIZE     SCREEN_HEIGHT * CHARS_ON_LINE


enum file_modes {
    NEW_FILE    = 1,
    UPDATE_FILE = 2
};


int main(int argc, char *argv[]);
void loop();
void display_text();