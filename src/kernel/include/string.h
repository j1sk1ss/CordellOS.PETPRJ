#pragma once

#include "stdio.h"

#include "../../libs/include/allocator.h"
#include "../../libs/include/math.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define DOUBLE_STR_BUFFER_SIZE 64


int isdigit(int c);
int isspace(int c);

const char* strchr(const char* str, char chr);
char* strrchr(const char *s, int c);
int strstr(const char* haystack, const char* needle);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int strcmp(const char* firstStr, const char* secondStr);
int strncmp(const char* str1, const char* str2, size_t n);
char* strcat(char* dest, const char* src);

char* strtok(char* s, const char* delim);
char* strtok_r(char* s, const char* delim, char** last);
void fit_string(char* str, size_t size, char character);
char place_char_in_text(char* text, char character, int x_position, int y_position);

void reverse(char* str, int len);
char* double_to_string(double value);
double atof(const char *str);

void backspace_string(char** str, size_t size);
void add_char_to_string(char** str, size_t size, char character);
void add_string_to_string(char** str, char* string);

wchar_t* utf16_to_codepoint(wchar_t*  strin, int* codePointg);
char* codepoint_to_utf8(int codePoint, char* stringOutput);

int	atoi(char *str);
char* itoa(int n);

char* strncpy(char *dst, const char *src, size_t n);
char* strdup(const char *src);

void str_uppercase(char* str);
int str_toupper(int c);
