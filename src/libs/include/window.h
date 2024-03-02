#include <stdint.h>

#include "stdlib.h"
#include "graphics.h"


struct window {
    char* name;

    int x, y;
    int height, width;
    uint32_t background_color;

    int is_active;
    struct GUIobject* objects;

    int pid;

    struct window* next;
};


struct window* create_window(char* name, int pid, int x, int y, int height, int width, uint32_t bg_color, struct GUIobject* objects);
void add_window(struct window* window);
void display_window(struct window* window);
void unload_window(struct window* window);

struct window* add_text2window(struct window* window, struct text_object* text);
struct window* add_object2window(struct window* window, struct GUIobject* object);
