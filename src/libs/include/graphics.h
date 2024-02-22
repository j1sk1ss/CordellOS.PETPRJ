#ifndef GRAPHICS_H_
#define GRAPHICS_H_


#include <stdint.h>

#include "bitmap.h"


#define BLACK       0x00000000 
#define WHITE       0x00FFFFFF 
#define DARK_GRAY   0x00222222
#define LIGHT_GRAY  0x00DDDDDD
#define RED         0x00FF0000 
#define GREEN       0x0000FF00 
#define BLUE        0x000000FF 
#define YELLOW      0x00FFFF00 
#define PURPLE      0x00FF00FF
#define TRANSPARENT 0x10000000


struct GUIobject {
    int x, y;
    int prev_x, prev_y;
    int height, width;

    uint32_t background_color;
    
    int children_count;
    struct GUIobject** childrens;

    int bitmap_count;
    struct bitmap** bitmaps;
};


void swipe_buffers();

int get_resolution_x();
int get_resolution_y();

struct GUIobject* create_gui_object(int x, int y, int height, int width, uint32_t background);
struct GUIobject* object_add_children(struct GUIobject* object, struct GUIobject* children);
struct GUIobject* object_add_bitmap(struct GUIobject* object, struct bitmap* bmp);

void object_move(struct GUIobject* object, int rel_x, int rel_y);

void display_gui_object(struct GUIobject* object);
void unload_gui_object(struct GUIobject* object);

void put_pixel(int x, int y, int color);
void vput_pixel(int x, int y, int color);
int get_pixel(int x, int y);

#endif