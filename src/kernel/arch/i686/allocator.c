#include <stddef.h>
#include <stdint.h>

#include "../../include/allocator.h"

#define MAX_PAGE_ALIGNED_ALLOCS 64

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

uint32_t alloc_start 	= 0;

uint32_t last_alloc     = 0;
uint32_t heap_end       = 0;
uint32_t heap_begin     = 0;
uint32_t pheap_begin    = 0;
uint32_t pheap_end      = 0;
uint8_t *pheap_desc     = 0;
uint32_t memory_used    = 0;

uint32_t get_memory() {
	kprintf("Memory used: %d bytes\n", memory_used);
	kprintf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used);
	kprintf("Heap size: %d bytes\n", heap_end - heap_begin);
	kprintf("Heap start: 0x%x\n", heap_begin);
	kprintf("Heap end: 0x%x\n", heap_end);
	kprintf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end);
	return last_alloc;
}

///////////////////////////////////////
//
//	ALLOCATORS INITIALIZATION
//
	void mm_init(uint32_t kernel_end) {
		alloc_start = kernel_end;

		last_alloc  = kernel_end;
		heap_begin  = last_alloc;
		pheap_end   = 0x1000000;
		pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
		heap_end    = pheap_begin;

		memset((char*)heap_begin, 0, heap_end - heap_begin);
		pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
	}

	void mm_reset() {
		last_alloc  = alloc_start;
		heap_begin  = last_alloc;
		pheap_end   = 0x1000000;
		pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
		heap_end    = pheap_begin;

		memset((char*)heap_begin, 0, heap_end - heap_begin);
		pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
	}
//
//	ALLOCATORS INITIALIZATION
//
//////////////////////////////////////
//
//	ALLOCATORS
//
	void free(void* mem) {
		alloc_t* alloc = (mem - sizeof(alloc_t));
		memory_used -= alloc->size + sizeof(alloc_t);
		alloc->status = 0;
	}

	void pfree(void* mem) {
		if(mem < pheap_begin || mem > pheap_end) return;

		/* Determine which page is it */
		uint32_t ad = (uint32_t)mem;
		ad -= pheap_begin;
		ad /= 4096;

		/* Now, ad has the id of the page */
		pheap_desc[ad] = 0;
		return;
	}

	void* malloc(size_t size) {
		if (!size) return 0;

		// Loop through blocks and find a block sized the same or bigger
		uint8_t *mem = (uint8_t*)heap_begin;
		while ((uint32_t)mem < last_alloc) {
			alloc_t *a = (alloc_t*)mem;

			// If the alloc has no size, we have reaced the end of allocation
			if (!a->size) goto nalloc;

			// If the alloc has a status of 1 (allocated), then add its size
			// and the sizeof alloc_t to the memory and continue looking.
			if (a->status == 1) {
				mem += a->size;
				mem += sizeof(alloc_t);
				mem += 4;

				continue;
			}

			// If the is not allocated, and its size is bigger or equal to the
			// requested size, then adjust its size, set status and return the location.
			if (a->size >= size) {
				a->status = 1;

				memset(mem + sizeof(alloc_t), 0, size);
				memory_used += size + sizeof(alloc_t);
				return (char*)(mem + sizeof(alloc_t));
			}

			// If it isn't allocated, but the size is not good, then
			// add its size and the sizeof alloc_t to the pointer and
			// continue;
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
		}

		nalloc:;
		if (last_alloc + size + sizeof(alloc_t) >= heap_end) {
			kprintf("Cannot allocate %d bytes! Out of memory.\n", size);
			return -1;
		}

		alloc_t* alloc  = (alloc_t*)last_alloc;
		alloc->status   = 1;
		alloc->size     = size;

		last_alloc += size;
		last_alloc += sizeof(alloc_t);
		last_alloc += 4;

		memory_used += size + 4 + sizeof(alloc_t);

		memset((char*)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
		return (char*)((uint32_t)alloc + sizeof(alloc_t));
	}

	void* pmalloc() {
		for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++) {
			if(pheap_desc[i]) continue;
			pheap_desc[i] = 1;
			return (char*)(pheap_begin + i * 4096);
		}
		
		kprintf("pmalloc: FATAL: failure!\n");
		return 0;
	}

	void* realloc(void* ptr, size_t size) {
		void* new_data = NULL;
		if (size) {
			if(!ptr) return malloc(size);

			new_data = malloc(size);
			if(new_data) {
				memcpy(new_data, ptr, size);
				free(ptr);
			}
		}

		return new_data;
	}

	void* calloc(size_t nelem, size_t elsize) {
		void* tgt = malloc(nelem * elsize);
		if (tgt != NULL) 
			memset(tgt, 0, nelem * elsize);

		return tgt;
	}
//
//	ALLOCATORS
//
//////////////////////////////////////