#include "../../libs/core/shared/include/allocator.h"
#include "../include/string.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define DOUBLE_STR_BUFFER_SIZE 64

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isspace(int c) {
    return ((c == '\n') || (c == '\t') || (c == ' '));
}

const char* strchr(const char* str, char chr) {
    if (str == NULL)
        return NULL;

    while (*str) {
        if (*str == chr)
            return str;

        ++str;
    }

    return NULL;
}

int strstr(const char* haystack, const char* needle) {
    if (*needle == '\0')    // If the needle is an empty string, return 0 (position 0).
        return 0;
    
    int position = 0;       // Initialize the position to 0.

    while (*haystack) {
        const char* hay_ptr     = haystack;
        const char* needle_ptr  = needle;

        // Compare characters in the haystack and needle.
        while (*hay_ptr == *needle_ptr && *needle_ptr) {
            hay_ptr++;
            needle_ptr++;
        }

        // If we reached the end of the needle, we found a match.
        if (*needle_ptr == '\0') 
            return position;

        // Move to the next character in the haystack.
        haystack++;
        position++;
    }

    return -1;  // Needle not found, return -1 to indicate that.
}

char* strcpy(char* dst, const char* src) {
    char* origDst = dst;

    if (dst == NULL)
        return NULL;

    if (src == NULL) {
        *dst = '\0';
        return dst;
    }

    while (*src) {
        *dst = *src;

        ++src;
        ++dst;
    }
    
    *dst = '\0';
    return origDst;
}

unsigned strlen(const char* str) {
    unsigned len = 0;
    while (*str) {
        ++len;
        ++str;
    }

    return len;
}

int strcmp(const char* firstStr, const char* secondStr) {
    if (firstStr == NULL && secondStr == NULL)
        return 0;

    if (firstStr == NULL || secondStr == NULL)
        return -10;

    while (*firstStr && *secondStr && *firstStr == *secondStr) {
        ++firstStr;
        ++secondStr;
    }

    return (*firstStr) - (*secondStr);
}

void reverse(char* str, int len) {
    int start = 0;
    int end = len - 1;
    char temp;

    while (start < end) {
        temp        = str[start];
        str[start]  = str[end];
        str[end]    = temp;

        start++;
        end--;
    }
}

double atof(const char *str) {
    double result       = 0.0;
    int sign            = 1;
    double fraction     = 0.0;
    bool is_decimal     = false;
    int decimal_places  = 0;

    // Skip whitespace characters
    while (isspace(*str)) 
        str++;

    // Determine sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') 
        str++;
    
    // Process the string
    while (*str != '\0') {
        if (*str == '.' && !is_decimal) {
            is_decimal = true;
            str++;
            continue;
        }

        if (isdigit(*str)) {
            if (is_decimal) {
                fraction = fraction * 10.0 + (*str - '0');
                decimal_places++;
            } else 
                result = result * 10.0 + (*str - '0');
        } else 
            break;

        str++;
    }

    // Combine integer and fractional parts
    result += fraction / pow(10.0, decimal_places);
    result *= sign;

    return result;
}

char* double_to_string(double value) {
    static char buffer[DOUBLE_STR_BUFFER_SIZE];

    int int_part            = (int)value;
    double fractional_part  = value - int_part;
    int i                   = 0;
    bool is_negative        = false;

    if (value < 0) {
        is_negative     = true;
        int_part        = -int_part;
        fractional_part = -fractional_part;
    }

    do {
        buffer[i++] = int_part % 10 + '0';
        int_part /= 10;
    } while (int_part != 0);

    if (is_negative) 
        buffer[i++] = '-';

    reverse(buffer, i);

    buffer[i++] = '.';

    int num_decimal_digits = 6;
    for (int j = 0; j < num_decimal_digits; j++) {
        fractional_part *= 10;
        int digit = (int)fractional_part;
        buffer[i++] = digit + '0';
        fractional_part -= digit;
    }

    buffer[i] = '\0';
    return buffer;
}

char* strcat(char* dest, const char* src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char* strtok(char* s, const char* delim) {
	static char* last;
	return strtok_r(s, delim, &last);
}

char* strtok_r(char* s, const char* delim, char** last) {
	char* spanp;
	int c, sc;
	char* tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);
	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
	    if (c == sc)
		    goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
	    *last = NULL;
		return (NULL);
	}

	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */

	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;

				*last = s;

				return (tok);
			}

		} while (sc != 0);
	}
}

void backspace_string(char** str, size_t size) {
    if (*str[size] == '\n') {
        VGA_putchr(cursor_get_x() - 1, cursor_get_y() - 1, ' ');
        VGA_setcursor(cursor_get_x() - 1, cursor_get_y() - 1); 
    } else {
        VGA_putchr(cursor_get_x() - 1, cursor_get_y(), ' ');
        VGA_setcursor(cursor_get_x() - 1, cursor_get_y()); 
    }

    char* buffer = (char*)malloc(size);
    memset(buffer, 0, sizeof(buffer));

    if (buffer == NULL) {
        free(buffer);
        return;
    }

    strcpy(buffer, *str);

    buffer[size] = '\0';

    free(*str);
    *str = buffer;   
}

void add_char_to_string(char** str, size_t size, char character) {
    char* buffer = (char*)malloc(size + 1);
    memset(buffer, 0, sizeof(buffer));
    
    if (buffer == NULL) {
        free(buffer);
        return;
    }
    
    strcpy(buffer, *str);

    buffer[size - 1] = character;            // Set last character
    buffer[size]     = '\0';                 // Null-terminate the string

    free(*str);
    *str = buffer;    
}

wchar_t* utf16_to_codepoint(wchar_t* string, int* codePoint) {
    int first = *string;
    ++string;

    if (first >= 0xd800 && first < 0xdc00) {
        int second = *string;
        ++string;

        *codePoint = ((first & 0x3ff) << 10) + (second & 0x3ff) + 0x10000;
    }

    *codePoint = first;

    return string;
}

char* codepoint_to_utf8(int codePoint, char* stringOutput) {
    if (codePoint <= 0x7F) 
        *stringOutput = (char)codePoint;
    else if (codePoint <= 0x7FF) {
        *stringOutput++ = 0xC0 | ((codePoint >> 6) & 0x1F);
        *stringOutput++ = 0xC0 | (codePoint & 0x3F);
    }
    else if (codePoint <= 0xFFFF) {
        *stringOutput++ = 0xE0 | ((codePoint >> 12) & 0xF);
        *stringOutput++ = 0x80 | ((codePoint >> 6) & 0x3F);
        *stringOutput++ = 0x80 | (codePoint & 0x3F);
    }
    else if (codePoint <= 0x1FFFFF) {
        *stringOutput++ = 0xF0 | ((codePoint >> 18) & 0x7);
        *stringOutput++ = 0x80 | ((codePoint >> 12) & 0x3F);
        *stringOutput++ = 0x80 | ((codePoint >> 6) & 0x3F);
        *stringOutput++ = 0x80 | (codePoint & 0x3F);
    }

    return stringOutput;
}

int	atoi(char *str) {
	int neg;
	int num;
	int i;

	i = 0;
	neg = 1;
	num = 0;

	while (str[i] <= ' ')
		i++;

	if (str[i] == '-' || str[i] == '+') {
		if (str[i] == '-') {
			neg *= -1;
		}

		i++;
	}

	while (str[i] >= '0' && str[i] <= '9') {
		num = num * 10 + (str[i] - 48);
		i++;
	}
    
	return (num * neg);
}