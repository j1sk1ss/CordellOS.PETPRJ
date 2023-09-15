#pragma once

#include <stddef.h>

const char* strchr(const char* str, char chr);
int strstr(const char* haystack, const char* needle);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int strcmp(const char* firstStr, const char* secondStr);

wchar_t* utf16_to_codepoint(wchar_t*  strin, int* codePointg);
char* codepoint_to_utf8(int codePoint, char* stringOutput);
