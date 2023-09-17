#pragma once

#include <stddef.h>

void initializeMemoryPool();
void* malloc(size_t size);
void* calloc(size_t num_elements, size_t element_size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);