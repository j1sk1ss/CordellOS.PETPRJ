#include "../include/gfx.h"

vbe_mode_info_t gfx_mode;

uint32_t chars[CHARCOUNT * CHARLEN];

void GFX_init(struct multiboot_info* mb_info) {
    gfx_mode.physical_base_pointer = mb_info->framebuffer_addr;
    gfx_mode.x_resolution          = mb_info->framebuffer_width;
    gfx_mode.y_resolution          = mb_info->framebuffer_height;
    gfx_mode.bits_per_pixel        = mb_info->framebuffer_bpp;
    gfx_mode.pitch                 = mb_info->framebuffer_pitch;

    gfx_mode.linear_red_mask_size         = mb_info->framebuffer_red_mask_size;
    gfx_mode.linear_red_field_position    = mb_info->framebuffer_red_field_position;

    gfx_mode.linear_green_mask_size       = mb_info->framebuffer_green_mask_size;
    gfx_mode.linear_green_mask_size       = mb_info->framebuffer_green_field_position;

    gfx_mode.linear_blue_mask_size        = mb_info->framebuffer_blue_mask_size;
    gfx_mode.linear_blue_field_position   = mb_info->framebuffer_blue_field_position;

    for(unsigned char c = ' '; c < '~'; c++) {
        unsigned short offset = (c - 31) * 16 ;
        for(int row = 0; row < CHAR_Y; row++) {
            uint8_t mask = 1 << 7;
            uint32_t *abs_row = chars + CHAROFF(c) + (row * 8);
            for(int i = 0; i < 8; i++) {
                if(font.Bitmap[offset + row] & mask) abs_row[i] = 0xFFFFFFFF;
                else abs_row[i] = BLACK;
                
                mask = (mask >> 1);
            }
        }
    }

    is_vesa = true;
}

void GFX_draw_pixel(uint16_t X, uint16_t Y, uint32_t color) {
    uint8_t* framebuffer    = (uint8_t*)gfx_mode.physical_base_pointer; 
    uint8_t bytes_per_pixel = (gfx_mode.bits_per_pixel + 1) / 8;

    framebuffer += (Y * gfx_mode.x_resolution + X) * bytes_per_pixel;

    for (uint8_t temp = 0; temp < bytes_per_pixel; temp++)
        framebuffer[temp] = (uint8_t)(color >> temp * 8);
}

void GFX_draw_line(Point start, Point end, uint32_t color) {
    int16_t deltaX = abs((end.X - start.X));
    int16_t deltaY = -abs((end.Y - start.Y));
    int16_t signX  = (start.X < end.X) ? 1 : -1;
    int16_t signY  = (start.Y < end.Y) ? 1 : -1;
    int16_t error  = deltaX + deltaY;
    int16_t errorX2;

    while (1) {
        GFX_draw_pixel(start.X, start.Y, color);
        if (start.X == end.X && start.Y == end.Y) break;

        errorX2 = error * 2;
        if (errorX2 >= deltaY) {
            error   += deltaY;
            start.X += signX;
        }
        if (errorX2 <= deltaX) {
            error   += deltaX;
            start.Y += signY;
        }
    }
}

void GFX_draw_triangle(Point vertex0, Point vertex1, Point vertex2, uint32_t color) {
    GFX_draw_line(vertex0, vertex1, color);
    GFX_draw_line(vertex1, vertex2, color);
    GFX_draw_line(vertex2, vertex0, color);
}

void GFX_draw_rect(Point top_left, Point bottom_right, uint32_t color) {
    Point temp = bottom_right;
    
    temp.Y = top_left.Y;
    GFX_draw_line(top_left, temp, color);
    GFX_draw_line(temp, bottom_right, color);

    temp.X = top_left.X;
    temp.Y = bottom_right.Y;
    GFX_draw_line(bottom_right, temp, color);
    GFX_draw_line(temp, top_left, color);
}

void GFX_draw_polygon(Point vertex_array[], uint8_t num_vertices, uint32_t color) {
    for (uint8_t i = 0; i < num_vertices - 1; i++)
        GFX_draw_line(vertex_array[i], vertex_array[i+1], color);

    GFX_draw_line(vertex_array[num_vertices - 1], vertex_array[0], color);
}

void GFX_draw_circle(Point center, uint16_t radius, uint32_t color) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t p = 1 - radius;

    GFX_draw_pixel(center.X + x, center.Y + y, color);
    GFX_draw_pixel(center.X - x, center.Y + y, color);
    GFX_draw_pixel(center.X + x, center.Y - y, color);
    GFX_draw_pixel(center.X - x, center.Y - y, color);
    GFX_draw_pixel(center.X + y, center.Y + x, color);
    GFX_draw_pixel(center.X - y, center.Y + x, color);
    GFX_draw_pixel(center.X + y, center.Y - x, color);
    GFX_draw_pixel(center.X - y, center.Y - x, color);

    while (x < y) {
        x++;
        if (p < 0) p += 2*x + 1;
        else {
            y--;
            p += 2*(x-y) + 1;
        }

        GFX_draw_pixel(center.X + x, center.Y + y, color);
        GFX_draw_pixel(center.X - x, center.Y + y, color);
        GFX_draw_pixel(center.X + x, center.Y - y, color);
        GFX_draw_pixel(center.X - x, center.Y - y, color);
        GFX_draw_pixel(center.X + y, center.Y + x, color);
        GFX_draw_pixel(center.X - y, center.Y + x, color);
        GFX_draw_pixel(center.X + y, center.Y - x, color);
        GFX_draw_pixel(center.X - y, center.Y - x, color);
    }
}

void GFX_boundary_fill(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color) {
    uint8_t* framebuffer    = (uint8_t *)gfx_mode.physical_base_pointer; 
    uint8_t bytes_per_pixel = (gfx_mode.bits_per_pixel + 1) / 8;
    uint8_t draw = 0;

    framebuffer += (Y * gfx_mode.x_resolution + X) * bytes_per_pixel;

    for (uint8_t temp = 0; temp < bytes_per_pixel; temp++) {
        if ((framebuffer[temp] != (uint8_t)(fill_color >> (temp * 8))) &&
            (framebuffer[temp] != (uint8_t)(boundary_color >> (temp * 8)))) {

            draw = 1;
            break;
        }
    }

    if (draw) {
        for (uint8_t temp = 0; temp < bytes_per_pixel; temp++)
            framebuffer[temp] = (uint8_t)(fill_color >> temp * 8);

        GFX_boundary_fill(X + 1, Y, fill_color, boundary_color);
        GFX_boundary_fill(X - 1, Y, fill_color, boundary_color);
        GFX_boundary_fill(X, Y + 1, fill_color, boundary_color);
        GFX_boundary_fill(X, Y - 1, fill_color, boundary_color);
    }
} 

void GFX_fill_triangle_solid(Point p0, Point p1, Point p2, uint32_t color) {
    Point temp;

    temp.X = (p0.X + p1.X + p2.X) / 3;
    temp.Y = (p0.Y + p1.Y + p2.Y) / 3;

    GFX_draw_triangle(p0, p1, p2, color - 1);
    GFX_boundary_fill(temp.X, temp.Y, color, color - 1);
    GFX_draw_triangle(p0, p1, p2, color);
}

void GFX_fill_rect_solid(Point top_left, Point bottom_right, uint32_t color) {
   for (uint16_t y = top_left.Y; y < bottom_right.Y; y++)
       for (uint16_t x = top_left.X; x < bottom_right.X; x++)
           GFX_draw_pixel(x, y, color);
}

void GFX_fill_polygon_solid(Point vertex_array[], uint8_t num_vertices, uint32_t color) {
    Point temp;

    temp.X = (vertex_array[0].X + vertex_array[1].X + vertex_array[2].X) / 3;
    temp.Y = (vertex_array[0].Y + vertex_array[1].Y + vertex_array[2].Y) / 3;

    GFX_draw_polygon(vertex_array, num_vertices, color - 1);
    GFX_boundary_fill(temp.X, temp.Y, color, color - 1);
    GFX_draw_polygon(vertex_array, num_vertices, color);
}

void GFX_fill_circle_solid(Point center, uint16_t radius, uint32_t color) {
    for (int16_t y = -radius; y <= radius; y++)
        for (int16_t x = -radius; x <= radius; x++)
            if (x*x + y*y < radius*radius - radius)
                GFX_draw_pixel(center.X + x, center.Y + y, color);
}

void GFX_put_char(int x, int y, int character, uint32_t foreground, uint32_t background) {
    uint32_t step = gfx_mode.pitch / 4;
    uint32_t* chardat = chars + CHAROFF(character);
    uint32_t* abs_row = ((unsigned char*)gfx_mode.physical_base_pointer) + (y * gfx_mode.pitch);
    abs_row += x;

    for(int row = 0; row < CHAR_Y * 8; row += 8) {
        memcpy(abs_row, chardat + row, 32);
        abs_row += step;
    }
}

uint32_t GFX_convert_color(const uint32_t color) {
    uint8_t convert_r, convert_g, convert_b;
    uint32_t converted_color = 0;

    const uint8_t orig_r = (color >> 16) & 0xFF;
    const uint8_t orig_g = (color >> 8)  & 0xFF;
    const uint8_t orig_b = color         & 0xFF;

    if (gfx_mode.bits_per_pixel == 8) {
        convert_r = 0;
        convert_g = 0;
        convert_b = orig_b;
    } else {
        const uint8_t r_bits_to_shift = 8 - gfx_mode.linear_red_mask_size; 
        const uint8_t g_bits_to_shift = 8 - gfx_mode.linear_green_mask_size;
        const uint8_t b_bits_to_shift = 8 - gfx_mode.linear_blue_mask_size;

        convert_r = (orig_r >> r_bits_to_shift) & ((1 << gfx_mode.linear_red_mask_size) - 1);
        convert_g = (orig_g >> g_bits_to_shift) & ((1 << gfx_mode.linear_green_mask_size) - 1);
        convert_b = (orig_b >> b_bits_to_shift) & ((1 << gfx_mode.linear_blue_mask_size) - 1);
    }

    converted_color = (convert_r << gfx_mode.linear_red_field_position)   |
                      (convert_g << gfx_mode.linear_green_field_position) |
                      (convert_b << gfx_mode.linear_blue_field_position);

    return converted_color;
}