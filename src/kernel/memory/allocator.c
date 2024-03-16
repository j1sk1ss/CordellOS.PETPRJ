#include "../include/allocator.h"


//===========================
//	GLOBAL VARS
//===========================

	malloc_block_t* kmalloc_list_head = NULL;
	malloc_block_t* umalloc_list_head = NULL;

	uint32_t malloc_virt_address     = 0x300000;
	uint32_t malloc_phys_address     = 0;
	uint32_t total_malloc_pages      = 0;
	page_directory* malloc_dir		 = 0;

//===========================
//	GLOBAL VARS
//===========================
//	INITIALIZATION
//	- Allocate page if needed
//	- Create first malloc block
//===========================

	void kmm_init(const uint32_t bytes) {
		malloc_dir = current_page_directory;
		total_malloc_pages = bytes / PAGE_SIZE;
		if (bytes % PAGE_SIZE > 0) total_malloc_pages++;

		malloc_phys_address = (uint32_t)allocate_blocks(total_malloc_pages);
		kmalloc_list_head    = (malloc_block_t*)malloc_virt_address;
		if (!malloc_phys_address) {
			kprintf("\n[%s %i] Allocated error\n", __FILE__, __LINE__);
			return;
		}

		for (uint32_t i = 0, virt = malloc_virt_address; i < total_malloc_pages; i++, virt += PAGE_SIZE) {
			map_page((void*)(malloc_phys_address + i * PAGE_SIZE), (void*)virt);
			pt_entry* page = get_page(virt);
			SET_ATTRIBUTE(page, PTE_READ_WRITE);
		}

		if (kmalloc_list_head != NULL) {
			kmalloc_list_head->v_addr = malloc_virt_address;
			kmalloc_list_head->pcount = total_malloc_pages;
			kmalloc_list_head->size   = (total_malloc_pages * PAGE_SIZE) - sizeof(malloc_block_t);
			kmalloc_list_head->free   = true;
			kmalloc_list_head->next   = NULL;
		}
	}

	void umm_init(const uint32_t bytes) {
		malloc_dir = current_page_directory;
		total_malloc_pages = bytes / PAGE_SIZE;
		if (bytes % PAGE_SIZE > 0) total_malloc_pages++;

		malloc_phys_address  = (uint32_t)allocate_blocks(total_malloc_pages);
		umalloc_list_head    = (malloc_block_t*)malloc_virt_address;
		if (!malloc_phys_address) {
			kprintf("\n[%s %i] Allocated error\n", __FILE__, __LINE__);
			return;
		}

		for (uint32_t i = 0, virt = malloc_virt_address; i < total_malloc_pages; i++, virt += PAGE_SIZE) {
			map_page2user((void*)(malloc_phys_address + i * PAGE_SIZE), (void*)virt);
			pt_entry* page = get_page(virt);
			SET_ATTRIBUTE(page, PTE_READ_WRITE);
		}

		if (umalloc_list_head != NULL) {
			umalloc_list_head->v_addr = malloc_virt_address;
			umalloc_list_head->pcount = total_malloc_pages;
			umalloc_list_head->size   = (total_malloc_pages * PAGE_SIZE) - sizeof(malloc_block_t);
			umalloc_list_head->free   = true;
			umalloc_list_head->next   = NULL;
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

	void block_split(malloc_block_t* node, const uint32_t size) {
		malloc_block_t* new_node = (malloc_block_t*)((void*)node + size + sizeof(malloc_block_t));

		new_node->size   = node->size - size - sizeof(malloc_block_t);
		new_node->free   = true;
		new_node->next   = node->next;
		new_node->v_addr = node->v_addr;

		node->size   = size;
		node->free   = false;
		node->next   = new_node;
		node->pcount -= (size / PAGE_SIZE) + 1;
	}
	
	void* kmallocp(uint32_t v_addr) {
		pt_entry page  = 0;
		uint32_t* temp = allocate_page(&page);
		map_page((void*)temp, (void*)v_addr);
		SET_ATTRIBUTE(&page, PTE_READ_WRITE);	
	}

	void* umallocp(uint32_t v_addr) {
		pt_entry page  = 0;
		uint32_t* temp = allocate_page(&page);
		map_page2user((void*)temp, (void*)v_addr);
		SET_ATTRIBUTE(&page, PTE_READ_WRITE);	
	}

	// Memory allocation in kernel address space. Usermode will cause error
	void* kmalloc(const uint32_t size) {
		if (size <= 0) return 0;
		if (kmalloc_list_head == NULL) kmm_init(size);

		//=============
		// Find a block
		//=============

			merge_free_blocks(kmalloc_list_head);
			malloc_block_t* cur = kmalloc_list_head;
			while (cur->next != NULL) {
				if (cur->free == true) {
					if (cur->size == size) break;
					if (cur->size > size + sizeof(malloc_block_t)) break;
				}
				
				cur = cur->next;
			}

		//=============
		// Find a block
		//=============
		// Work with block
		//=============
		
			if (size == cur->size) cur->free = false;
			else if (cur->size > size + sizeof(malloc_block_t)) block_split(cur, size);
			else {
				//=============
				// Allocate new page
				//=============
				
					uint8_t num_pages = 1;
					while (cur->size + num_pages * PAGE_SIZE < size + sizeof(malloc_block_t))
						num_pages++;

					uint32_t virt = malloc_virt_address + total_malloc_pages * PAGE_SIZE; // TODO: new pages to new blocks. Don`t mix them to avoid pagedir errors in contswitch
					for (uint8_t i = 0; i < num_pages; i++) {
						kmallocp(virt);

						virt += PAGE_SIZE;
						cur->size += PAGE_SIZE;
						total_malloc_pages++;
					}

					block_split(cur, size);

				//=============
				// Allocate new page
				//=============
			}
		
		//=============
		// Work with block
		//=============

		return (void*)cur + sizeof(malloc_block_t);
	}

	void* krealloc(void* ptr, size_t size) {
		void* new_data = NULL;
		if (size) {
			if(!ptr) return kmalloc(size);

			new_data = kmalloc(size);
			if(new_data) {
				memcpy(new_data, ptr, size);
				kfree(ptr);
			}
		}

		return new_data;
	}

	void* umalloc(const uint32_t size) {
		if (size <= 0) return 0;
		if (umalloc_list_head == NULL) umm_init(size);

		//=============
		// Find a block
		//=============

			merge_free_blocks(umalloc_list_head);
			malloc_block_t* cur = umalloc_list_head;
			while (cur->next != NULL) {
				if (cur->free == true) {
					if (cur->size == size) break;
					if (cur->size > size + sizeof(malloc_block_t)) break;
				}
				
				cur = cur->next;
			}

		//=============
		// Find a block
		//=============
		// Work with block
		//=============
		
			if (size == cur->size) cur->free = false;
			else if (cur->size > size + sizeof(malloc_block_t)) block_split(cur, size);
			else {
				//=============
				// Allocate new page
				//=============
				
					uint8_t num_pages = 1;
					while (cur->size + num_pages * PAGE_SIZE < size + sizeof(malloc_block_t))
						num_pages++;

					uint32_t virt = malloc_virt_address + total_malloc_pages * PAGE_SIZE; // TODO: new pages to new blocks. Don`t mix them to avoid pagedir errors in contswitch
					for (uint8_t i = 0; i < num_pages; i++) {
						umallocp(virt);

						virt += PAGE_SIZE;
						cur->size += PAGE_SIZE;
						total_malloc_pages++;
					}

					block_split(cur, size);

				//=============
				// Allocate new page
				//=============
			}
		
		//=============
		// Work with block
		//=============

		return (void*)cur + sizeof(malloc_block_t);
	}

	void merge_free_blocks(malloc_block_t* block) {
		malloc_block_t* cur = block;
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
		if (ptr == NULL) return;
		for (malloc_block_t* cur = kmalloc_list_head; cur->next; cur = cur->next) 
			if ((void*)cur + sizeof(malloc_block_t) == ptr && cur->free == false) {
				cur->free = true;
				memset(ptr, 0, cur->size);
				merge_free_blocks(kmalloc_list_head);

				break;
			}

		for (malloc_block_t* cur = kmalloc_list_head; cur->next; cur = cur->next) {
			if ((void*)cur + sizeof(malloc_block_t) == ptr && cur->free == false) {
				uint32_t num_pages = cur->pcount;
				for (uint32_t i = 0; i < num_pages; i++) {
					uint32_t v_addr = cur->v_addr + i * PAGE_SIZE;
					kfreep((void*)v_addr);
				}

				// Mark the block as free and clear memory content
				cur->free = true;
				memset(ptr, 0, cur->size);

				// Merge adjacent free blocks
				merge_free_blocks(kmalloc_list_head);
				break;
			}
		}
	}

	void ufree(void* ptr) {
		if (ptr == NULL) return;
		for (malloc_block_t* cur = umalloc_list_head; cur->next; cur = cur->next) 
			if ((void*)cur + sizeof(malloc_block_t) == ptr && cur->free == false) {
				cur->free = true;
				memset(ptr, 0, cur->size);
				merge_free_blocks(umalloc_list_head);

				break;
			}

		for (malloc_block_t* cur = umalloc_list_head; cur->next; cur = cur->next) {
			if ((void*)cur + sizeof(malloc_block_t) == ptr && cur->free == false) {
				uint32_t num_pages = cur->pcount;
				for (uint32_t i = 0; i < num_pages; i++) {
					uint32_t v_addr = cur->v_addr + i * PAGE_SIZE;
					kfreep((void*)v_addr);
				}

				// Mark the block as free and clear memory content
				cur->free = true;
				memset(ptr, 0, cur->size);

				// Merge adjacent free blocks
				merge_free_blocks(umalloc_list_head);
				break;
			}
		}
	}

	void kfreep(void* v_addr) {
		pt_entry* page = get_page((virtual_address)v_addr);
		if (PAGE_PHYS_ADDRESS(page) && TEST_ATTRIBUTE(page, PTE_PRESENT)) {
			free_page(page);
			unmap_page((uint32_t*)v_addr);
			flush_tlb_entry((virtual_address)v_addr);
		}
	}

//===========================
//	ALLOC FUNCTIONS
//===========================
//	INFO
//===========================

	uint32_t kmalloc_total_free() {
		uint32_t total_free = 0;
		if (kmalloc_list_head->free = true) total_free += kmalloc_list_head->size + sizeof(malloc_block_t);
		for (malloc_block_t* cur = kmalloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				if (cur->next->free == true) total_free += cur->next->size + sizeof(malloc_block_t);
		
		return total_free;
	}

	uint32_t kmalloc_total_avaliable() {
		uint32_t total_free = kmalloc_list_head->size + sizeof(malloc_block_t);
		for (malloc_block_t* cur = kmalloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				total_free += cur->next->size + sizeof(malloc_block_t);
		
		return total_free;
	}

	void print_kmalloc_map() {
		kprintf(
			"\n|%i(%c)|",
			kmalloc_list_head->size + sizeof(malloc_block_t),
			kmalloc_list_head->free == true ? 'F' : 'O'
		);

		for (malloc_block_t* cur = kmalloc_list_head; cur->next; cur = cur->next)
			if (cur->next != NULL)
				kprintf(
					"%i(%c)|",
					cur->next->size + sizeof(malloc_block_t),
					cur->next->free == true ? 'F' : 'O'
				);

		kprintf(" TOTAL: [%iB]\n", kmalloc_total_avaliable());
	}

//===========================
//	INFO
//===========================