#pragma once

#include <stdint.h>
#include <stddef.h>

#include "memory.h"
// #include "phys_manager.h"
// #include "virt_manager.h"

#define PAGE_SIZE               4096
#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t get_memory();

void mm_init(uint32_t kernel_end);
void mm_reset();

void* kmalloc(size_t size);
void* kpmalloc();

void* krealloc(void* ptr, size_t size);
void* kcalloc(size_t nelem, size_t elsize);

void kfree(void *mem);
void kpfree(void* mem);

void consolidate_free_blocks();
