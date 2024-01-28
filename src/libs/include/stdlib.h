#pragma once

#include <stdint.h>

#include "fatlib.h"
#include "memory.h"

#define PAGE_SIZE 4096

void tstart(char* name, uint32_t address);
void tkill();

void sleep(int milliseconds);
void get_datetime(short* data);

void* malloc(uint32_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nelem, size_t elsize);
void free(void* ptr);

int cexists(const char* path);
void rmcontent(const char* path, const char* name);
void chgcontent(const char* path, struct udirectory_entry* meta);

char* fread(const char* path);
void fwrite(const char* path, const char* data);
void mkfile(const char* path, const char* name);
int fexec(char* path, int args, char** argv);

struct UFATDirectory* opendir(const char* path);
void mkdir(const char* path, const char* name);