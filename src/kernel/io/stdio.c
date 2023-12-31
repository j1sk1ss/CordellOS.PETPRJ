#include "../include/stdio.h"
#include <include/io.h>

#include <stdarg.h>
#include <stdbool.h>

#include <include/vfs.h>


void fputc(char c, fileDescriptorId file, int color) {
    if (color == 1)
        cputc(c, file);
    else
        VFS_Write(file, &c, sizeof(c));
}

void cputc(char c, uint8_t color) {
    VFS_Color_Write(color, &c, sizeof(c));
}

void fputs(const char* str, fileDescriptorId file) {
    while(*str) {
        fputc(*str, file, 0);
        str++;
    }
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

const char _HexChars[] = "0123456789abcdef";

void fprintf_unsigned(fileDescriptorId file, unsigned long long number, int radix) {
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do  {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = _HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        fputc(buffer[pos], file, 0);
}

void fprintf_signed(fileDescriptorId file, long long number, int radix) {
    if (number < 0) {
        fputc('-', file, 0);
        fprintf_unsigned(file, -number, radix);
    }
    else 
        fprintf_unsigned(file, number, radix);
}

void vfprintf(fileDescriptorId file, const char* fmt, va_list args, int color) {
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
                        fputc(*fmt, file, color);
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
                    length  = PRINTF_LENGTH_SHORT_SHORT;
                    state   = PRINTF_STATE_SPEC;
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
                        fputc((char)va_arg(args, int), file, color);
                    break;

                    case 's':   
                        fputs(va_arg(args, const char*), file);
                    break;

                    case '%':   
                        fputc('%', file, color);
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

                    // ignore invalid spec
                    default:    
                        break;
                }

                if (number) {
                    if (sign) {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                fprintf_signed(file, va_arg(args, int), radix);
                            break;

                            case PRINTF_LENGTH_LONG:        
                                fprintf_signed(file, va_arg(args, long), radix);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                fprintf_signed(file, va_arg(args, long long), radix);
                            break;
                        }
                    }
                    else {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                fprintf_unsigned(file, va_arg(args, unsigned int), radix);
                            break;
                                                            
                            case PRINTF_LENGTH_LONG:        
                                fprintf_unsigned(file, va_arg(args, unsigned  long), radix);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                fprintf_unsigned(file, va_arg(args, unsigned  long long), radix);
                            break;
                        }
                    }
                }

                // reset state
                state   = PRINTF_STATE_NORMAL;
                length  = PRINTF_LENGTH_DEFAULT;
                radix   = 10;
                sign    = false;
                number  = false;

                break;
        }

        fmt++;
    }
}

void fprintf(fileDescriptorId file, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args, 0);
    va_end(args);
}

void fprint_buffer(fileDescriptorId file, const char* msg, const void* buffer, uint32_t count) {
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    fputs(msg, file);
    for (uint16_t i = 0; i < count; i++) {
        fputc(_HexChars[u8Buffer[i] >> 4], file, 0);
        fputc(_HexChars[u8Buffer[i] & 0xF], file, 0);
    }

    fputs("\n", file);
}

void putc(char c) {
    fputc(c, VFS_FD_STDOUT, 0);
}

void puts(const char* str) {
    fputs(str, VFS_FD_STDOUT);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(VFS_FD_STDOUT, fmt, args, 0);
    va_end(args);
}

void cprintf(uint8_t color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(color, fmt, args, 1);
    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count) {
    fprint_buffer(VFS_FD_STDOUT, msg, buffer, count);
}

void debugc(char c) {
    fputc(c, VFS_FD_DEBUG, 0);
}

void debugs(const char* str) {
    fputs(str, VFS_FD_DEBUG);
}

void debugf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(VFS_FD_DEBUG, fmt, args, 0);
    va_end(args);
}

void debug_buffer(const char* msg, const void* buffer, uint32_t count) {
    fprint_buffer(VFS_FD_DEBUG, msg, buffer, count);
}