/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>

#include "../include/paging.h"

static uint32_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
static uint32_t* last_page = 0;

/* Paging now will be really simple
 * we reserve 0-8MB for kernel stuff
 * heap will be from approx 1mb to 4mb
 * and paging stuff will be from 4mb
 */
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys) {
	uint16_t id = virt >> 22;
	for(int i = 0; i < 1024; i++) {
		last_page[i] = phys | 3;
		phys += 4096;
	}
    
	page_directory[id] = ((uint32_t)last_page) | 3;
	last_page = (uint32_t *)(((uint32_t)last_page) + 4096);
}

void paging_enable() {
	asm("mov %%eax, %%cr3": :"a"(page_dir_loc));	
	asm("mov %cr0, %eax");
	asm("orl $0x80000000, %eax");
	asm("mov %eax, %cr0");
}

void paging_init(uint32_t kernel_end) {
	page_directory = (uint32_t*)kernel_end;
	page_dir_loc   = (uint32_t)page_directory;
	last_page      = (uint32_t *)kernel_end + 0x4000;

	for(int i = 0; i < 1024; i++) page_directory[i] = 0 | 2;
	
	paging_map_virtual_to_phys(0, 0);
	paging_map_virtual_to_phys(kernel_end, kernel_end);
	paging_enable();
}