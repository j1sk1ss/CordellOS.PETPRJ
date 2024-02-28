#include "../include/stdio.h"


bool is_vesa = false;


void kclrscr() {
    if (is_vesa) {
        VESA_clrscr();
        // GFX_buffer2buffer();
    }
    else VGA_clrscr();
}

void kfputc(char c, uint8_t file, int color) {
    if (color == 1) kcputc(c, file);
    else {
        if (!is_vesa) VGA_putc(c);
        else VESA_putc(c);
    }
}

void kcputc(char c, uint8_t color) {
    if (!is_vesa) {
        VGA_putc(c);
        VGA_putcolor(VGA_cursor_get_x() - 1, VGA_cursor_get_y(), color);
    } else VESA_cputc(c, color, BLACK);
}

void kfputs(const char* str, uint8_t file, int color) {
    while(*str) {
        kfputc(*str, file, color);
        str++;
    }
}

void kset_color(int color) {
    if (!is_vesa) VGA_set_color(color);
    else {
        Point fpoint, spoint;

        fpoint.X = 0;
        fpoint.Y = 0;
        spoint.X = gfx_mode.x_resolution;
        spoint.Y = gfx_mode.y_resolution;

        GFX_fill_rect_solid(fpoint, spoint, color);
    }
}

const char _HexChars[] = "0123456789ABCDEF";

void kfprintf_unsigned(uint8_t file, unsigned long long number, int radix, int color) {
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do  {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = _HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0) kfputc(buffer[pos], file, color);
}

void kfprintf_signed(uint8_t file, long long number, int radix, int color) {
    if (number < 0) {
        kfputc('-', file, color);
        kfprintf_unsigned(file, -number, radix, color);
    }
    else kfprintf_unsigned(file, number, radix, color);
}

void kvfprintf(uint8_t file, const char* fmt, va_list args, int color) {
    int state   = PRINTF_STATE_NORMAL;
    int length  = PRINTF_LENGTH_DEFAULT;
    int radix   = 10;

    bool sign   = false;
    bool number = false;

    while (*fmt) {
        switch (state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt) {
                    case '%':   
                        state = PRINTF_STATE_LENGTH;
                    break;

                    default:    
                        kfputc(*fmt, file, color);
                    break;
                }

                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt) {
                    case 'h':   
                        length  = PRINTF_LENGTH_SHORT;  
                        state   = PRINTF_STATE_LENGTH_SHORT;
                    break;

                    case 'l':   
                        length  = PRINTF_LENGTH_LONG;
                        state   = PRINTF_STATE_LENGTH_LONG;
                    break;

                    default:    
                        goto PRINTF_STATE_SPEC_;
                }

                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h') {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state  = PRINTF_STATE_SPEC;
                }
                else 
                    goto PRINTF_STATE_SPEC_;

                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l') {
                    length  = PRINTF_LENGTH_LONG_LONG;
                    state   = PRINTF_STATE_SPEC;
                }
                else 
                    goto PRINTF_STATE_SPEC_;

                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt) {
                    case 'c':   
                        kfputc((char)va_arg(args, int), file, color);
                    break;

                    case 's':   
                        kfputs(va_arg(args, const char*), file, color);
                    break;

                    case '%':   
                        kfputc('%', file, color);
                    break;

                    case 'd':
                    case 'i':   
                        radix   = 10; 
                        sign    = true; 
                        number  = true;
                    break;

                    case 'u':   
                        radix   = 10; 
                        sign    = false; 
                        number  = true;
                    break;

                    case 'X':
                    case 'x':
                    case 'p':   
                        radix   = 16; 
                        sign    = false; 
                        number  = true;
                    break;

                    case 'o':   
                        radix   = 8; 
                        sign    = false; 
                        number  = true;
                    break;

                    default: break;
                }

                if (number) {
                    if (sign) {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                kfprintf_signed(file, va_arg(args, int), radix, color);
                            break;

                            case PRINTF_LENGTH_LONG:        
                                kfprintf_signed(file, va_arg(args, long), radix, color);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                kfprintf_signed(file, va_arg(args, long long), radix, color);
                            break;
                        }
                    }
                    else {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                kfprintf_unsigned(file, va_arg(args, unsigned int), radix, color);
                            break;
                                                            
                            case PRINTF_LENGTH_LONG:        
                                kfprintf_unsigned(file, va_arg(args, unsigned  long), radix, color);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                kfprintf_unsigned(file, va_arg(args, unsigned  long long), radix, color);
                            break;
                        }
                    }
                }

                // reset state
                state  = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix  = 10;
                sign   = false;
                number = false;

                break;
        }

        fmt++;
    }
}

void kfprintf(uint8_t file, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvfprintf(file, fmt, args, 0);
    va_end(args);
}

void kfprint_buffer(uint8_t file, const char* msg, const void* buffer, uint32_t count) {
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    kfputs(msg, file, 0);
    for (uint16_t i = 0; i < count; i++) {
        kfputc(_HexChars[u8Buffer[i] >> 4], file, 0);
        kfputc(_HexChars[u8Buffer[i] & 0xF], file, 0);
        kfputc(" ", file, 0);
    }

    kfputs("\n", file, 0);
}

void kputc(char c) {
    kfputc(c, NULL, 0);
}

void kputs(const char* str) {
    kfputs(str, NULL, 0);
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvfprintf(NULL, fmt, args, 0);
    va_end(args);
}

void kcprintf(uint8_t color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvfprintf(color, fmt, args, 1);
    va_end(args);
}

void kprint_buffer(const char* msg, const void* buffer, uint32_t count) {
    kfprint_buffer(NULL, msg, buffer, count);
}

void kprint_hex_table(const char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        kprintf("%c%c ", '0' + ((unsigned char)*data >> 4), '0' + ((unsigned char)*data & 0x0F));
        if ((i + 1) % 16 == 0 || i == length - 1) kprintf("\n");
        data++;
    }
}