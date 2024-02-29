#include "../include/window.h"


struct window* windows = NULL;


struct window* create_window(char* name, int pid, int x, int y, int height, int width, uint32_t bg_color, struct GUIobject* objects) {
    struct window* window = malloc(sizeof(struct window));

    window->name    = name;
    window->next    = NULL;
    window->x       = x;
    window->y       = y;
    window->height  = height;
    window->width   = width;
    window->objects = objects;
    window->pid     = pid;
    window->background_color = bg_color;

    if (objects != NULL) object_move(window->objects, window->x, window->y);
    return window;
}

void add_window(struct window* window) {
    if (windows == NULL) windows = window;
    else {
        struct window* cur = windows;
        while (cur->next != NULL) 
            cur = cur->next;

        cur->next = window;
    }
}

void display_window(struct window* window) {
    struct GUIobject* window_image   = create_gui_object(window->x, window->y, window->height, window->width, window->background_color);
    struct GUIobject* title_image    = create_gui_object(window->x, window->y, 25, window->width, BLUE);
    struct text_object* window_title = create_text(window->x + 10, window->y + 5, window->name, BLUE);

    window_image = object_add_children(window_image, title_image);
    window_image = object_add_text(window_image, window_title);

    display_gui_object(window_image);
    if (window->objects != NULL) display_gui_object(window->objects);

    unload_gui_object(window_image);
}

void unload_window(struct window* window) {
    if (window->objects != NULL) unload_gui_object(window->objects);
    free(window);
}