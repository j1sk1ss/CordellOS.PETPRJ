#include <include/io.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

const unsigned int SCREEN_WIDTH  = 80;      //  Default widgth of screens
const unsigned int SCREEN_HEIGHT = 25;      //  Default height of screens
const uint8_t DEFAULT_COLOR      = 0x7;     //  Default color

uint8_t* _screenBuffer = (uint8_t*)0xB8000; // Position of screen buffer in memory

int _screenX = 0;                           //  Cursor position
int _screenY = 0;                           //

int cursor_get_x() {
    return _screenX;
}

int cursor_get_y() {
    return _screenY;
}

//
//  Get char from screen buffer
//
char VGA_getchr(int x, int y) {
    return (char)_screenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

//
//  Put character to screen buffer
//
void VGA_putchr(int x, int y, char c) {
    _screenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

//
//  Get char colot in current coordinates
//
uint8_t VGA_getcolor(int x, int y) {
    return _screenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

//
//  Put color in current coordinates
//
void VGA_putcolor(int x, int y, uint8_t color) {
    _screenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

//
//  Set console cursore into x and y coordinates
//  X and Y between default screen widght and screen height
//  Check it on file top
//
void VGA_setcursor(int x, int y) {
    uint16_t pos = y * SCREEN_WIDTH + x;

    i686_outb(0x3D4, 0x0F);                          // First value is port on VGA, second - value 
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));         // for this register
    i686_outb(0x3D4, 0x0E);                          // Check of. docs for info about this ports
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));  //

    _screenX = x;
    _screenY = y;
}

//
//  Clear screen
//
void VGA_clrscr() {
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            VGA_putchr(x, y, 0);
            VGA_putcolor(x, y, DEFAULT_COLOR);
        }

    _screenX = 0;
    _screenY = 0;

    VGA_setcursor(_screenX, _screenY);
}

//
//  Scroll all screen down
//
void VGA_scrollback(int lines) {
    for (int y = lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++){
            VGA_putchr(x, y - lines, VGA_getchr(x, y));
            VGA_putcolor(x, y - lines, VGA_getcolor(x, y));
        }

    for (int x = 0; x < SCREEN_WIDTH; x++){
        VGA_putchr(x, SCREEN_HEIGHT - lines, 0);
        VGA_putcolor(x, SCREEN_HEIGHT - lines, DEFAULT_COLOR);
    }

    _screenY -= lines;
}

//
//  Put character and check special symbols
//
void VGA_putc(char c) {
    const int _tabSize = 4;

    switch (c) {
        case '\n':                                          // New line
            _screenX = 0;
            _screenY++;
        break;
    
        case '\t':                                          // Tabulation
            for (int i = 0; i < _tabSize - (_screenX % _tabSize); i++)
                VGA_putc(' ');
        break;

        case '\r':                                          // Line start
            _screenX = 0;
        break;

        default:                                            // Write character
            VGA_putchr(_screenX , _screenY, c);
                _screenX += 1;
        break;
    }

    if (_screenX >= SCREEN_WIDTH) {                         // Next line when we reach the end of screen
        _screenY++;
        _screenX = 0;
    }

    if (_screenY >= SCREEN_HEIGHT)
        VGA_scrollback(1);

    VGA_setcursor(_screenX , _screenY);
}