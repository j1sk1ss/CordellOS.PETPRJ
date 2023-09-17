#include "allocator.h"

// Define a block size for memory allocation
#define BLOCK_SIZE 16

// A simple structure to represent a memory block
struct MemoryBlock {
    size_t size;
    struct MemoryBlock* next;
};

// Define a memory pool
static char memoryPool[1024]; // Change the size according to your requirements

// Define a pointer to the start of the free memory
static struct MemoryBlock* freeList = NULL;

// Function to initialize the memory pool
void initializeMemoryPool() {
    freeList = (struct MemoryBlock*)memoryPool;

    freeList->size = sizeof(memoryPool);
    freeList->next = NULL;
}

// Function to de-initialize the memory pool
void deInitializeMemoryPool() {
    freeList = NULL;
}

// Function to allocate memory
void* malloc(size_t size) {
    if (size == 0) return NULL;

    // Find a block large enough to hold the requested size
    struct MemoryBlock* prev = NULL;
    struct MemoryBlock* curr = freeList;

    while (curr != NULL) {
        if (curr->size >= size) {
            if (curr->size > size + sizeof(struct MemoryBlock)) {
                // Split the block if it's larger than required
                struct MemoryBlock* newBlock = (struct MemoryBlock*)((char*)curr + size);
            
                newBlock->size = curr->size - size - sizeof(struct MemoryBlock);
                newBlock->next = curr->next;

                curr->size = size;
                curr->next = newBlock;
            }

            if (prev != NULL) prev->next = curr->next;
            else freeList = curr->next;

            return (char*)curr + sizeof(struct MemoryBlock);
        }

        prev = curr;
        curr = curr->next;
    }

    // No suitable block found
    return NULL;
}

// Function to allocate and zero-initialize memory
void* calloc(size_t num_elements, size_t element_size) {
    size_t size = num_elements * element_size;
    void* ptr = malloc(size);

    if (ptr != NULL) 
        for (size_t i = 0; i < size; i++) // Zero out the allocated memory
            *((char*)ptr + i) = 0;

    return ptr;
}

// Function to reallocate memory
void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) 
        return malloc(size);
    
    struct MemoryBlock* block = (struct MemoryBlock*)((char*)ptr - sizeof(struct MemoryBlock));

    if (size == 0) {
        free(ptr);
        return NULL;
    }
    else if (size <= block->size) 
        return ptr;
    else {
        void* newPtr = malloc(size);
        if (newPtr != NULL) {
            // Copy the data from the old block to the new block
            for (size_t i = 0; i < block->size; i++) 
                *((char*)newPtr + i) = *((char*)ptr + i);

            free(ptr);

            return newPtr;
        } 
        else 
            return NULL;
    }
}

// Function to deallocate memory
void free(void* ptr) {
    if (ptr == NULL) return;

    struct MemoryBlock* block = (struct MemoryBlock*)((char*)ptr - sizeof(struct MemoryBlock));

    block->next = freeList;
    freeList    = block;
}