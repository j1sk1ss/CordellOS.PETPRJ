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
void* mallocp(uint32_t v_addr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nelem, size_t elsize);
void free(void* ptr);
void freep(void* ptr);

int cexists(const char* path);
void rmcontent(const char* path, const char* name);
void chgcontent(const char* path, udirectory_entry_t* meta);

char* fread(const char* path);
void fread_off(UContent* content, int offset, uint8_t* buffer, int len);
void fwrite(const char* path, const char* data);
void mkfile(const char* path, const char* name);
int fexec(char* path, int args, char** argv);

UDirectory* opendir(const char* path);
UContent* get_content(const char* path);
void mkdir(const char* path, const char* name);

void machine_restart();
void switch_disk(int index);

void get_fs_info(uint32_t* buffer);
