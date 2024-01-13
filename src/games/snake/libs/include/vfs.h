#pragma once

#include <stdint.h>
#include <stddef.h>

typedef int fileDescriptorId;   // Indetifine file

#define VFS_FD_STDIN        0
#define VFS_FD_STDOUT       1
#define VFS_FD_STDERR       2
#define VFS_FD_DEBUG        3

    /////////////////
    //  COLORS

        // General Formatting
        #define GEN_FORMAT_RESET        0x00
        #define GEN_FORMAT_BRIGHT       0x01
        #define GEN_FORMAT_DIM          0x02
        #define GEN_FORMAT_UNDERSCORE   0x03
        #define GEN_FORMAT_BLINK        0x04
        #define GEN_FORMAT_REVERSE      0x05
        #define GEN_FORMAT_HIDDEN       0x06

        // Foreground Colors
        #define FOREGROUND_BLACK        0x00
        #define FOREGROUND_BLUE         0x01
        #define FOREGROUND_GREEN        0x02
        #define FOREGROUND_AQUA         0x03
        #define FOREGROUND_RED          0x04
        #define FOREGROUND_PURPLE       0x05
        #define FOREGROUND_YELLOW       0x06
        #define FOREGROUND_WHITE        0x07
        #define FOREGROUND_GREY         0x08
        #define FOREGROUND_LIGHT_BLUE   0x09
        #define FOREGROUND_LIGHT_GREEN  0x0A
        #define FOREGROUND_LIGHT_AQUA   0x0B
        #define FOREGROUND_LIGHT_RED    0x0C
        #define FOREGROUND_LIGHT_PURPLE 0x0D
        #define FOREGROUND_LIGHT_YELLOW 0x0E
        #define FOREGROUND_BRIGHT_WHITE 0x0F

        // Background Colors
        #define BACKGROUND_BLACK        0x00
        #define BACKGROUND_BLUE         0x10
        #define BACKGROUND_GREEN        0x20
        #define BACKGROUND_AQUA         0x30
        #define BACKGROUND_RED          0x40
        #define BACKGROUND_PURPLE       0x50
        #define BACKGROUND_YELLOW       0x60
        #define BACKGROUND_WHITE        0x70

    //  COLORS
    /////////////////

int VFS_write(fileDescriptorId file, uint8_t* data, size_t size);
int VFS_color_write(uint8_t color, uint8_t* data, size_t size);

void VFS_set_screen_color(uint8_t color);