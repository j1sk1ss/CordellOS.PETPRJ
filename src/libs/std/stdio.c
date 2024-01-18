#include "../include/stdio.h"

void clrscr() {
    SYS_clrs();
}

void fputc(char c, uint8_t file, int color) {
    if (color == 0) SYS_putc(c);
    else cputc(c, file);
}

void cputc(char c, uint8_t color) {
    SYS_cputc(color, c);
}

void fputs(const char* str, uint8_t file, int color) {
    while(*str) {
        fputc(*str, file, color);
        str++;
    }
}

void set_color(int color) {
    SYS_scrclr(color);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


void fprintf_unsigned(uint8_t file, unsigned long long number, int radix, int color) {
    char hexChars[17] = "0123456789abcdef";
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do  {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = hexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        fputc(buffer[pos], file, color);
}

void fprintf_signed(uint8_t file, long long number, int radix, int color) {
    if (number < 0) {
        fputc('-', file, color);
        fprintf_unsigned(file, -number, radix, color);
    }
    else 
        fprintf_unsigned(file, number, radix, color);
}

void vfprintf(uint8_t file, const char* fmt, va_list args, int color) {
    int state   = PRINTF_STATE_NORMAL;
    int length  = PRINTF_LENGTH_DEFAULT;
    int radix   = 10;
    bool sign   = false;
    bool number = false;

    while (*fmt) {
        if (state == PRINTF_STATE_NORMAL) {
            switch (*fmt) {
                case '%':   
                    state = PRINTF_STATE_LENGTH;
                break;

                default:    
                    fputc(*fmt, file, color);
                break;
            }
        }

        else if (state == PRINTF_STATE_LENGTH) {
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
        }

        else if (state == PRINTF_STATE_LENGTH_SHORT) {
            if (*fmt == 'h') {
                length  = PRINTF_LENGTH_SHORT_SHORT;
                state   = PRINTF_STATE_SPEC;
            }
            else 
                goto PRINTF_STATE_SPEC_;           
        }

        else if (state == PRINTF_STATE_LENGTH_LONG) {
            if (*fmt == 'l') {
                length  = PRINTF_LENGTH_LONG_LONG;
                state   = PRINTF_STATE_SPEC;
            }
            else 
                goto PRINTF_STATE_SPEC_;            
        }

        else if (state == PRINTF_STATE_SPEC) {
            PRINTF_STATE_SPEC_:
            if (*fmt == 'c') fputc((char)va_arg(args, int), file, color);
            else if (*fmt == 's') fputs(va_arg(args, const char*), file, color);
            else if (*fmt == '%') fputc('%', file, color);
            
            else if (*fmt == 'd' || *fmt == 'i') {
                radix   = 10; 
                sign    = true; 
                number  = true;
            }

            else if (*fmt == 'u') {
                radix   = 10; 
                sign    = false; 
                number  = true;
            }

            else if (*fmt == 'X' || *fmt == 'x' || *fmt == 'p') {
                radix   = 16; 
                sign    = false; 
                number  = true;
            }

            else if (*fmt == 'o') {
                radix   = 8; 
                sign    = false; 
                number  = true;
            }

            if (number == true) {
                if (sign == true) {
                    if (length == PRINTF_LENGTH_SHORT_SHORT || length == PRINTF_LENGTH_SHORT || length == PRINTF_LENGTH_DEFAULT) 
                        fprintf_signed(file, va_arg(args, int), radix, color);
                    
                    else if (length == PRINTF_LENGTH_LONG)
                        fprintf_signed(file, va_arg(args, long), radix, color);

                    else if (length == PRINTF_LENGTH_LONG_LONG)
                        fprintf_signed(file, va_arg(args, long long), radix, color);
                }
                else {
                    if (length == PRINTF_LENGTH_SHORT_SHORT || length == PRINTF_LENGTH_SHORT || length == PRINTF_LENGTH_DEFAULT) 
                        fprintf_unsigned(file, va_arg(args, unsigned int), radix, color);
                    
                    else if (length == PRINTF_LENGTH_LONG)
                        fprintf_unsigned(file, va_arg(args, unsigned long), radix, color);

                    else if (length == PRINTF_LENGTH_LONG_LONG)
                        fprintf_unsigned(file, va_arg(args, unsigned long long), radix, color);
                }
            }

            // reset state
            state   = PRINTF_STATE_NORMAL;
            length  = PRINTF_LENGTH_DEFAULT;
            radix   = 10;
            sign    = false;
            number  = false;            
        }

        fmt++;
    }
}

void fprintf(uint8_t file, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args, 0);
    va_end(args);
}

void fprint_buffer(uint8_t file, const char* msg, const void* buffer, uint32_t count) {
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    char hexChars[17] = "0123456789abcdef";
    
    fputs(msg, file, 0);
    for (uint16_t i = 0; i < count; i++) {
        fputc(hexChars[u8Buffer[i] >> 4], file, 0);
        fputc(hexChars[u8Buffer[i] & 0xF], file, 0);
        fputc(" ", file, 0);
    }

    fputs("\n", file, 0);
}

void putc(char c) {
    fputc(c, NULL, 0);
}

void puts(const char* str) {
    fputs(str, NULL, 0);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(NULL, fmt, args, 0);
    va_end(args);
}

void cprintf(uint8_t color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(color, fmt, args, 1);
    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count) {
    fprint_buffer(NULL, msg, buffer, count);
}

void print_hex_table(const char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%c%c ", '0' + ((unsigned char)data[i] >> 4), '0' + ((unsigned char)data[i] & 0x0F));
        if ((i + 1) % 16 == 0 || i == length - 1) printf("\n");
    }
}