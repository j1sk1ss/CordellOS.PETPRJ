#pragma once

#include <memory.h>
#include <stdint.h>

#include "stdio.h"


#define MEM_OFFSET      0x10000
#define BLOCK_SIZE      4096     // Size of 1 block of memory, 4KB
#define BLOCKS_PER_BYTE 8        // Using a bitmap, each byte will hold 8 bits/blocks


// Global variables
extern uint32_t* memory_map;
extern uint32_t max_blocks;
extern uint32_t used_blocks;


void PMM_init(const uint32_t start_address, const uint32_t size);
void initialize_memory_region(const uint32_t base_address, const uint32_t size);
void deinitialize_memory_region(const uint32_t base_address, const uint32_t size);

void set_block(const uint32_t bit);
void unset_block(const uint32_t bit);

int32_t find_first_free_blocks(const uint32_t num_blocks);

uint32_t *allocate_blocks(const uint32_t num_blocks);
void free_blocks(const uint32_t *address, const uint32_t num_blocks);