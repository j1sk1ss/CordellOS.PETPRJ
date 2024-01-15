#include <stdint.h>
#include <stddef.h>

#include "memory.h"

uint32_t get_memory();

void mm_init(uint32_t kernel_end);
void free(void *mem);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nelem, size_t elsize);