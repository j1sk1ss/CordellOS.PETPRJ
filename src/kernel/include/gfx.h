#pragma once

#include <stdint.h>

#include "stdio.h"

#include "../../libs/include/x86.h"

// 32 bit ARGB colors
#define BLACK      0x00000000 
#define WHITE      0x00FFFFFF 
#define DARK_GRAY  0x00222222
#define LIGHT_GRAY 0x00DDDDDD
#define RED        0x00FF0000 
#define GREEN      0x0000FF00 
#define BLUE       0x000000FF 
#define YELLOW     0x00FFFF00 
#define PURPLE     0x00FF00FF

#define abs(a) ((a > 0) ? a : -a)
#define ROUND(a) ((int)(a + 0.5))

// VBE Mode info block - holds current graphics mode values
typedef struct {
    // Mandatory info for all VBE revisions
	uint16_t mode_attributes;
	uint8_t window_a_attributes;
	uint8_t window_b_attributes;
	uint16_t window_granularity;
	uint16_t window_size;
	uint16_t window_a_segment;
	uint16_t window_b_segment;
	uint32_t window_function_pointer;
	uint16_t bytes_per_scanline;

    // Mandatory info for VBE 1.2 and above
	uint16_t x_resolution;
	uint16_t y_resolution;
	uint8_t x_charsize;
	uint8_t y_charsize;
	uint8_t number_of_planes;
	uint8_t bits_per_pixel;
	uint8_t number_of_banks;
	uint8_t memory_model;
	uint8_t bank_size;
	uint8_t number_of_image_pages;
	uint8_t reserved1;

    // Direct color fields (required for direct/6 and YUV/7 memory models)
	uint8_t red_mask_size;
	uint8_t red_field_position;
	uint8_t green_mask_size;
	uint8_t green_field_position;
	uint8_t blue_mask_size;
	uint8_t blue_field_position;
	uint8_t reserved_mask_size;
	uint8_t reserved_field_position;
	uint8_t direct_color_mode_info;

    // Mandatory info for VBE 2.0 and above
	uint32_t physical_base_pointer;         // Physical address for flat memory frame buffer
	uint32_t reserved2;
	uint16_t reserved3;

    // Mandatory info for VBE 3.0 and above
	uint16_t linear_bytes_per_scanline;
    uint8_t bank_number_of_image_pages;
    uint8_t linear_number_of_image_pages;
    uint8_t linear_red_mask_size;
    uint8_t linear_red_field_position;
    uint8_t linear_green_mask_size;
    uint8_t linear_green_field_position;
    uint8_t linear_blue_mask_size;
    uint8_t linear_blue_field_position;
    uint8_t linear_reserved_mask_size;
    uint8_t linear_reserved_field_position;
    uint32_t max_pixel_clock;

    uint8_t reserved4[190];              // Remainder of mode info block

} __attribute__ ((packed)) vbe_mode_info_t;

typedef struct {
    char signature[4];
    uint16_t version;
    uint32_t oem_string_ptr;
    uint32_t capabilities;
    uint32_t video_mode_ptr;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name_ptr;
    uint32_t oem_product_name_ptr;
    uint32_t oem_product_rev_ptr;
    uint8_t reserved[222];
    uint8_t oem_data[256];
} __attribute__((packed)) vbe_controller_info_t;

typedef struct {
    uint32_t fg_color;
    uint32_t bg_color;
} user_gfx_info_t;

typedef struct point {
    uint16_t X, Y;
} Point;

extern vbe_mode_info_t *gfx_mode;
extern user_gfx_info_t *user_gfx_info;

void GFX_init();
void GFX_draw_pixel(uint16_t X, uint16_t Y, uint32_t color);
void GFX_draw_line(Point start, Point end, uint32_t color);
void GFX_draw_triangle(Point vertex0, Point vertex1, Point vertex2, uint32_t color);
void GFX_draw_rect(Point top_left, Point bottom_right, uint32_t color);
void GFX_draw_polygon(Point vertex_array[], uint8_t num_vertices, uint32_t color);
void GFX_draw_circle(Point center, uint16_t radius, uint32_t color);
void GFX_boundary_fill(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color);
void GFX_fill_triangle_solid(Point p0, Point p1, Point p2, uint32_t color);
void GFX_fill_rect_solid(Point top_left, Point bottom_right, uint32_t color);
void GFX_fill_polygon_solid(Point vertex_array[], uint8_t num_vertices, uint32_t color);
void GFX_fill_circle_solid(Point center, uint16_t radius, uint32_t color);
uint32_t GFX_convert_color(const uint32_t color);