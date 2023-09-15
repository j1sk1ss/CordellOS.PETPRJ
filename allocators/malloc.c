#include "malloc.h"

/*
 * The find_suitable_block function traverses through the linked list
 * and find the first block which has space atleast equal to the requested space.
 * It also sets the variable last to the address of the last block in the linked-list.
 * This comes handy in the malloc function
 */
meta_ptr find_suitable_block(meta_ptr *last, size_t size) {
    meta_ptr b = base;
    while (b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }

    return last;
}

/*
 * The split_space function splits the given block if it contains space greater than the requested space.
 * Creates a new block of the free space and adds it in the linked list
 */
void split_space(meta_ptr block, size_t size) {
    meta_ptr new_block;

    new_block       = block->data + size;
    new_block->size = block->size - size - META_BLOCK_SIZE;
    new_block->next = block->next;
    new_block->free = 1;
    new_block->ptr  = new_block->data;
    new_block->prev = block;
    block->next     = new_block;
    block->size     = size;

    if (new_block->next)
        new_block->next->prev = new_block;
}

/*
 * extend_heap() is invoked when the already available blocks of memory is not sutiable or if no block exist already.
 * This creates a block of memory near the break of heap.
 * The meta_block of the newly created memory block will be added in the last of the linked list.
 */
meta_ptr extend_heap(meta_ptr last, size_t size) {
    meta_ptr old_break, new_break;
    old_break = sbrk(0);
    new_break = sbrk(META_BLOCK_SIZE + size);

    if (new_break == (void *)-1)
        return NULL;
    
    old_break->size = size;
    old_break->free = 0;
    old_break->next = NULL;
    old_break->prev = NULL;
    old_break->ptr  = old_break->data;

    if (last)
        last->next = old_break;

    return (old_break);
}

/*
 * malloc is the function which will be invoked by the user to allocate space.
 * The function first checks if there a memory block with space atleast equal to the requested space.
 * If not then it requests a new block to be created from the heap.
 * If there are no elements in the linked-list then it asks the heap to allocate memory block
 * Finally it return the address from where the data can be stored.
 */
void *malloc(size_t size) {
    meta_ptr block, last;
    size_t s;
    s = allign4(size);
    if (base) {
        last    = base;
        block   = find_suitable_block(&last, s);

        if (block) {
            if (block->size - s >= (META_BLOCK_SIZE + 4)) 
                split_space(block, s);
            
            block->free = 0;
        }
        else {
            block = extend_heap(last, s);
            if (!block)
                return NULL;
        }
    }
    else {
        block = extend_heap(NULL, s);
        if (!block) 
            return NULL;

        base = block;
    }

    return block->data;
}