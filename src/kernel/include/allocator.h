#include <stdint.h>
#include <stddef.h>

#include "memory.h"

#define PAGE_SIZE 4096

uint32_t get_memory();

void mm_init(uint32_t kernel_end);
void mm_reset();

void* malloc(size_t size);
void* pmalloc();

void* realloc(void* ptr, size_t size);
void* calloc(size_t nelem, size_t elsize);

void free(void *mem);
void pfree(void* mem);