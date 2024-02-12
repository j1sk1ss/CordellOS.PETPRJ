#include "../include/vesa_text.h"

int cursor_x = 0;
int cursor_y = 0;

void VESA_shift_up() {
    memcpy(((char*)gfx_mode.physical_base_pointer) + ((gfx_mode.x_resolution * 16) * 4), ((char *)gfx_mode.physical_base_pointer) + (
        (gfx_mode.x_resolution * 16) * 8), (gfx_mode.x_resolution * gfx_mode.y_resolution) * 4 - ((gfx_mode.x_resolution * 16) * 4));
    memset(((char*)gfx_mode.physical_base_pointer) + (gfx_mode.x_resolution * gfx_mode.y_resolution * 4), 0, gfx_mode.x_resolution * 16 * 4);
}

void VESA_newline() {
    cursor_x = 0;
    if(cursor_y >= gfx_mode.y_resolution) {
        cursor_y = gfx_mode.y_resolution;
        VESA_shift_up();
    }
    else cursor_y += 16;
}

void VESA_putc(char c) {
    if(cursor_x + 8 >= gfx_mode.x_resolution) 
        VESA_newline();

    switch (c) {
        case '\n':
            VESA_newline();
            break;

        case '\t':
            VESA_putc(' ');
            VESA_putc(' ');
            VESA_putc(' ');
            VESA_putc(' ');
            break;

        case BACKSPACE_BUTTON:
            if(cursor_x > 0) {
                cursor_x -= 8;
                VESA_putc(' ');
                cursor_x -= 8;
            }

            break;

        default:
            GFX_put_char(cursor_x, cursor_y, c, 0xFFFFFFFF, 0);
            cursor_x += 8;
            break;
    }
}

void VESA_set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x * 8;
    cursor_y = y * 16;
}