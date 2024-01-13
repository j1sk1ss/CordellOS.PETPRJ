#include "../include/vfs.h"
#include "../include/vga_text.h"


int VFS_write(fileDescriptorId file, uint8_t* data, size_t size) {
    switch (file) {
        case 0:
            return 0;

        case 1:
        case 2:
            for (size_t i = 0; i < size; i++)
                VGA_putc(data[i]);

            return size;

        default:
            return -1;
    }
}

int VFS_color_write(uint8_t color, uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        VGA_putc(data[i]);
        VGA_putcolor(VGA_cursor_get_x() - 1, VGA_cursor_get_y(), color);
    }

    return size;
}

void VFS_set_screen_color(uint8_t color) {
    VGA_set_color(color);
}