#pragma once

#include "gfx.h"
#include "keyboard.h"

#include "../../libs/include/memory.h"


void VESA_clrscr();

void VESA_newline();

void VESA_putc(char c);
void VESA_cputc(char c, uint32_t color);

void VESA_backspace();
void VESA_set_cursor(uint8_t x, uint8_t y);