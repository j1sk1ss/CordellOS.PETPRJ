#include <stdint.h>
#include <stddef.h>

#include "memory.h"



void mm_init(uint32_t kernel_end);
void free(void *mem);
void pfree(void *mem);
char* pmalloc(size_t size);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nelem, size_t elsize);

void heap_init();
int kalloc(int);