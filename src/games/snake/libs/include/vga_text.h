#pragma once

extern uint8_t* _screenBuffer; 

// Important functions
void VGA_clrscr();
void VGA_set_color(uint8_t color);
void VGA_putc(char c);

char VGA_getchr(int x, int y);

void VGA_putchr(int x, int y, char c);

uint8_t VGA_getcolor(int x, int y);

void VGA_putcolor(int x, int y, uint8_t color);
void VGA_setcursor(int x, int y);
void VGA_scrollback(int lines);

int VGA_cursor_get_x();
int VGA_cursor_get_y();
void VGA_cursor_place_to_line();
