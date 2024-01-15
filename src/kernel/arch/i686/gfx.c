#include "../../include/gfx.h"

vbe_mode_info_t* gfx_mode;
user_gfx_info_t* user_gfx_info;

void GFX_init() {
}

void get_vesa_info(vbe_controller_info_t *controller_info, vbe_mode_info_t *mode_info) {

}

void GFX_draw_pixel(uint16_t X, uint16_t Y, uint32_t color) {
    uint8_t *framebuffer    = (uint8_t *)gfx_mode->physical_base_pointer; 
    uint8_t bytes_per_pixel = (gfx_mode->bits_per_pixel+1) / 8;             // Get # of bytes per pixel, add 1 to fix 15bpp modes

    framebuffer += (Y * gfx_mode->x_resolution + X) * bytes_per_pixel;

    for (uint8_t temp = 0; temp < bytes_per_pixel; temp++)
        framebuffer[temp] = (uint8_t)(color >> temp * 8);
}

// Draw a line
//   Adapted from Wikipedia page on Bresenham's line algorithm
void GFX_draw_line(Point start, Point end, uint32_t color) {
    int16_t deltaX = abs((end.X - start.X));    // Delta X, change in X values, positive absolute value
    int16_t deltaY = -abs((end.Y - start.Y));   // Delta Y, change in Y values, negative absolute value
    int16_t signX = (start.X < end.X) ? 1 : -1; // Sign of X direction, moving right (positive) or left (negative)
    int16_t signY = (start.Y < end.Y) ? 1 : -1; // Sign of Y direction, moving down (positive) or up (negative)
    int16_t error = deltaX + deltaY;
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

// Draw a triangle
void GFX_draw_triangle(Point vertex0, Point vertex1, Point vertex2, uint32_t color) {
    // Draw lines between all 3 points
    GFX_draw_line(vertex0, vertex1, color);
    GFX_draw_line(vertex1, vertex2, color);
    GFX_draw_line(vertex2, vertex0, color);
}

// Draw a rectangle
void GFX_draw_rect(Point top_left, Point bottom_right, uint32_t color) {
    Point temp = bottom_right;
    
    // Draw 4 lines, 2 horizontal parallel sides, 2 vertical parallel sides
    // Top of rectangle
    temp.Y = top_left.Y;
    GFX_draw_line(top_left, temp, color);

    // Right side
    GFX_draw_line(temp, bottom_right, color);

    // Bottom
    temp.X = top_left.X;
    temp.Y = bottom_right.Y;
    GFX_draw_line(bottom_right, temp, color);

    // Left side
    GFX_draw_line(temp, top_left, color);
}

// Draw a polygon
void GFX_draw_polygon(Point vertex_array[], uint8_t num_vertices, uint32_t color) {
    // Draw lines up to last line
    for (uint8_t i = 0; i < num_vertices - 1; i++)
        GFX_draw_line(vertex_array[i], vertex_array[i+1], color);

    // Draw last line
    GFX_draw_line(vertex_array[num_vertices - 1], vertex_array[0], color);
}

// Draw a circle
//  Adapted from "Computer Graphics Principles and Practice in C 2nd Edition"
void GFX_draw_circle(Point center, uint16_t radius, uint32_t color) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t p = 1 - radius;

    // Draw initial 8 octant points
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

        // Draw next set of 8 octant points
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

// Fill an area with a solid color
void GFX_boundary_fill(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color) {
    // Recursive - may use a lot of stack space
    uint8_t *framebuffer    = (uint8_t *)gfx_mode->physical_base_pointer; 
    uint8_t bytes_per_pixel = (gfx_mode->bits_per_pixel+1) / 8;             // Get # of bytes per pixel, add 1 to fix 15bpp modes
    uint8_t draw = 0;

    framebuffer += (Y * gfx_mode->x_resolution + X) * bytes_per_pixel;

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

        // Check 4 pixels around current pixel
        GFX_boundary_fill(X + 1, Y, fill_color, boundary_color);
        GFX_boundary_fill(X - 1, Y, fill_color, boundary_color);
        GFX_boundary_fill(X, Y + 1, fill_color, boundary_color);
        GFX_boundary_fill(X, Y - 1, fill_color, boundary_color);
    }
} 


// Fill triangle with a solid color
void GFX_fill_triangle_solid(Point p0, Point p1, Point p2, uint32_t color) {
    Point temp;

    // Get center (Centroid) of a triangle
    temp.X = (p0.X + p1.X + p2.X) / 3;
    temp.Y = (p0.Y + p1.Y + p2.Y) / 3;

    // First draw triangles sides (boundaries)
    GFX_draw_triangle(p0, p1, p2, color - 1);

    // Then fill in boundaries 
    GFX_boundary_fill(temp.X, temp.Y, color, color - 1);

    // Then redraw boundaries to correct color
    GFX_draw_triangle(p0, p1, p2, color);
}

// Fill rectangle with a solid color
void GFX_fill_rect_solid(Point top_left, Point bottom_right, uint32_t color) {
   // Brute force method 
   for (uint16_t y = top_left.Y; y < bottom_right.Y; y++)
       for (uint16_t x = top_left.X; x < bottom_right.X; x++)
           GFX_draw_pixel(x, y, color);
}

// Fill a polygon with a solid color
void GFX_fill_polygon_solid(Point vertex_array[], uint8_t num_vertices, uint32_t color) {
    Point temp;

    // Assuming this works in general, get center (Centroid) of a triangle in the polygon
    temp.X = (vertex_array[0].X + vertex_array[1].X + vertex_array[2].X) / 3;
    temp.Y = (vertex_array[0].Y + vertex_array[1].Y + vertex_array[2].Y) / 3;

    // First draw polygon sides (boundaries)
    GFX_draw_polygon(vertex_array, num_vertices, color - 1);

    // Then fill in boundaries 
    GFX_boundary_fill(temp.X, temp.Y, color, color - 1);

    // Then redraw boundaries to correct color
    GFX_draw_polygon(vertex_array, num_vertices, color);
}

// Fill a circle with a solid color
void GFX_fill_circle_solid(Point center, uint16_t radius, uint32_t color) {
    // Brute force method
    for (int16_t y = -radius; y <= radius; y++)
        for (int16_t x = -radius; x <= radius; x++)
            if (x*x + y*y < radius*radius - radius)
                GFX_draw_pixel(center.X + x, center.Y + y, color);
}

// Convert given 32bit 888ARGB color to set bpp value 
// 0x00RRGGBB
uint32_t GFX_convert_color(const uint32_t color) {
    uint8_t convert_r, convert_g, convert_b;
    uint32_t converted_color = 0;

    // Get original color portions
    const uint8_t orig_r = (color >> 16) & 0xFF;
    const uint8_t orig_g = (color >> 8)  & 0xFF;
    const uint8_t orig_b = color         & 0xFF;

    if (gfx_mode->bits_per_pixel == 8) {
        // 8bpp uses standard VGA 256 color pallette
        // User can enter any 8bit value for color 0x00-0xFF
        convert_r = 0;
        convert_g = 0;
        convert_b = orig_b;
    } else {
        // Assuming bpp is > 8 and <= 32
        const uint8_t r_bits_to_shift = 8 - gfx_mode->linear_red_mask_size; 
        const uint8_t g_bits_to_shift = 8 - gfx_mode->linear_green_mask_size;
        const uint8_t b_bits_to_shift = 8 - gfx_mode->linear_blue_mask_size;

        // Convert to new color portions by getting ratio of bit sizes of color compared to "full" 8 bit colors
        convert_r = (orig_r >> r_bits_to_shift) & ((1 << gfx_mode->linear_red_mask_size) - 1);
        convert_g = (orig_g >> g_bits_to_shift) & ((1 << gfx_mode->linear_green_mask_size) - 1);
        convert_b = (orig_b >> b_bits_to_shift) & ((1 << gfx_mode->linear_blue_mask_size) - 1);
    }

    // Put new color portions into new color
    converted_color = (convert_r << gfx_mode->linear_red_field_position)   |
                      (convert_g << gfx_mode->linear_green_field_position) |
                      (convert_b << gfx_mode->linear_blue_field_position);

    return converted_color;
}