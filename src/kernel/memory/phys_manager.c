/* https://github.com/queso-fuego/amateuros/blob/master/include/memory/physical_memory_manager.h#L80 */

#include "../include/phys_manager.h"

// Sets a block/bit in the memory map (mark block as used)
void set_block(const uint32_t bit) {
    memory_map[bit / 32] |= (1u << (bit % 32));
}

// Unsets a block/bit in the memory map (mark block as free)
void unset_block(const uint32_t bit) {
    memory_map[bit / 32] &= ~(1u << (bit % 32));
}

// Find the first free blocks of memory for a given size
int32_t find_first_free_blocks(const uint32_t num_blocks) {
    if (num_blocks == 0) return -1;

    for (uint32_t i = 0; i < max_blocks / 32;  i++) {
        if (memory_map[i] != 0xFFFFFFFFu) {
            for (int32_t j = 0; j < 32; j++) {
                int32_t bit = 1u << j;

                if (!(memory_map[i] & bit)) {
                    for (uint32_t count = 0, free_blocks = 0; count < num_blocks; count++) {
                        if ((j + count > 31) && (i + 1 <= max_blocks / 32)) {
                            if (!(memory_map[i + 1] & (1u << ((j + count) - 32)))) free_blocks++;
                        } 
                        else {
                            if (!(memory_map[i] & (1u << (j + count)))) free_blocks++;
                        }

                        if (free_blocks == num_blocks) return i * 32 + j;
                    }
                }
            }
        }
    }

    return -1;
}

void initialize_memory_manager(const uint32_t start_address, const uint32_t size) {
    memory_map  = (uint32_t*)start_address;
    max_blocks  = size / BLOCK_SIZE;
    used_blocks = max_blocks;

    memset(memory_map, 0xFF, max_blocks / BLOCKS_PER_BYTE);
}

void initialize_memory_region(const uint32_t base_address, const uint32_t size) {
    int32_t align      = base_address / BLOCK_SIZE;
    int32_t num_blocks = size / BLOCK_SIZE;

    for (; num_blocks > 0; num_blocks--) {
        unset_block(align++);
        used_blocks--;
    }
}

void deinitialize_memory_region(const uint32_t base_address, const uint32_t size) {
    int32_t align      = base_address / BLOCK_SIZE;
    int32_t num_blocks = size / BLOCK_SIZE;

    for (; num_blocks > 0; num_blocks--) {
        set_block(align++);
        used_blocks++;
    }
}

uint32_t* allocate_blocks(const uint32_t num_blocks) {
    if ((max_blocks - used_blocks) <= num_blocks) return 0;   

    int32_t starting_block = find_first_free_blocks(num_blocks);
    if (starting_block == -1) return NULL;     // Couldn't find that many blocks in a row to allocate

    for (uint32_t i = 0; i < num_blocks; i++)
        set_block(starting_block + i);

    used_blocks += num_blocks;  // Blocks are now used/reserved, increase count

    uint32_t address = starting_block * BLOCK_SIZE + (uint32_t)memory_map;
    return (uint32_t*)address;  // Physical memory location of allocated blocks
}

// Free blocks memory
void free_blocks(const uint32_t *address, const uint32_t num_blocks) {
    int32_t starting_block = (uint32_t)address / BLOCK_SIZE;   // Convert address to blocks 

    for (uint32_t i = 0; i < num_blocks; i++) 
        unset_block(starting_block + i);    // Unset bits/blocks in memory map, to free

    used_blocks -= num_blocks;  // Decrease used block count
}