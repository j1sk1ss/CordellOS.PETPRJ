#include "../include/allocator.h"

//===========================
//	GLOBAL VARS
//===========================

	malloc_block_t* malloc_list_head = NULL;
	uint32_t malloc_virt_address     = 0x300000;
	uint32_t malloc_phys_address     = 0;
	uint32_t total_malloc_pages      = 0;

//===========================
//	GLOBAL VARS
//===========================
//	INITIALIZATION
//	- Allocate page if needed
//	- Create first malloc block
//===========================

	void mm_init(const uint32_t bytes) {
		total_malloc_pages = bytes / PAGE_SIZE;
		if (bytes % PAGE_SIZE > 0) total_malloc_pages++;

		malloc_phys_address = (uint32_t)allocate_blocks(total_malloc_pages);
		malloc_list_head    = (malloc_block_t*)malloc_virt_address;
		if (malloc_phys_address == NULL) {
			kprintf("Allocated error\n");
			return;
		}

		for (uint32_t i = 0, virt = malloc_virt_address; i < total_malloc_pages; i++, virt += PAGE_SIZE) {
			map_page((void*)(malloc_phys_address + i * PAGE_SIZE), (void*)virt);
			pt_entry* page = get_page(virt);
			SET_ATTRIBUTE(page, PTE_READ_WRITE);
		}

		if (malloc_list_head != NULL) {
			malloc_list_head->size = (total_malloc_pages * PAGE_SIZE) - sizeof(malloc_block_t);
			malloc_list_head->free = true;
			malloc_list_head->next = NULL;
		}
	}

//===========================
//	INITIALIZATION
//===========================
//	ALLOC FUNCTIONS
//	- Splitting for splitting malloc blocks
//	- KMalloc for allocating space
//	- Merge free blocks for merging blocks in page
//===========================

	uint32_t kmalloc_total_free() {
		uint32_t total_free = 0;
		if (malloc_list_head->free = true) total_free += malloc_list_head->size + sizeof(malloc_block_t);
		for (malloc_block_t* cur = malloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				if (cur->next->free == true) total_free += cur->next->size + sizeof(malloc_block_t);
		
		return total_free;
	}

	uint32_t kmalloc_total_avaliable() {
		uint32_t total_free = malloc_list_head->size + sizeof(malloc_block_t);
		for (malloc_block_t* cur = malloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				total_free += cur->next->size + sizeof(malloc_block_t);
		
		return total_free;
	}

	void print_kmalloc_map() {
		kprintf(
			"\n|%iB(%i)|",
			malloc_list_head->size + sizeof(malloc_block_t),
			malloc_list_head->free == true ? 1 : 0
		);

		for (malloc_block_t* cur = malloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				kprintf(
					"%iB(%i)|",
					cur->next->size + sizeof(malloc_block_t),
					cur->next->free == true ? 1 : 0
				);

		kprintf(" TOTAL: [%iB]\n", kmalloc_total_avaliable());
	}

	void kmalloc_split(malloc_block_t* node, const uint32_t size) {
		malloc_block_t* new_node = (malloc_block_t*)((void*)node + size + sizeof(malloc_block_t));

		new_node->size = node->size - size - sizeof(malloc_block_t);
		new_node->free = true;
		new_node->next = node->next;

		node->size = size;
		node->free = false;
		node->next = new_node;
	}
	
	void* kmalloc(const uint32_t size) {
		if (size <= 0) return 0;
		if (malloc_list_head == NULL) mm_init(size);

		malloc_block_t* cur = malloc_list_head;
		while (((cur->size < (size + sizeof(malloc_block_t))) || cur->free == false) && cur->next != NULL) cur = cur->next;
		
		if (size == cur->size) cur->free = false;
		else if (cur->size > size + sizeof(malloc_block_t)) kmalloc_split(cur, size);
		else {
			uint8_t num_pages = 1;
			while (cur->size + num_pages * PAGE_SIZE < size + sizeof(malloc_block_t))
				num_pages++;

			uint32_t virt = malloc_virt_address + total_malloc_pages * PAGE_SIZE;
			for (uint8_t i = 0; i < num_pages; i++) {
				pt_entry page = 0;
				uint32_t* temp = allocate_page(&page);

				map_page((void*)temp, (void*)virt);
				SET_ATTRIBUTE(&page, PTE_READ_WRITE); // TODO: new page create error (present)

				virt += PAGE_SIZE;
				cur->size += PAGE_SIZE;
				total_malloc_pages++;
			}

			kmalloc_split(cur, size);
		}
		
		return (void*)cur + sizeof(malloc_block_t);
	}

	void merge_free_blocks(void) {
		malloc_block_t* cur = malloc_list_head;
		while (cur != NULL && cur->next != NULL) {
			if (cur->free == true && cur->next->free == true) {
				cur->size += (cur->next->size) + sizeof(malloc_block_t);
				
				if (cur->next->next != NULL) cur->next = cur->next->next;
				else {
					cur->next = NULL;
					break;
				}
			}

			cur = cur->next;
		}
	}

	void kfree(void* ptr) {
		for (malloc_block_t* cur = malloc_list_head; cur->next; cur = cur->next) 
			if ((void*)cur + sizeof(malloc_block_t) == ptr && cur->free == false) {
				cur->free = true;
				merge_free_blocks();
				break;
			}
	}

//===========================
//	ALLOC FUNCTIONS
//===========================