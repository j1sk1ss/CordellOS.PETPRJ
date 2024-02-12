#pragma once

#include "gfx.h"
#include "keyboard.h"

#include "../../libs/include/memory.h"


void VESA_shift_up();
void VESA_newline();
void VESA_putc(char c);
void VESA_set_cursor(uint8_t x, uint8_t y);