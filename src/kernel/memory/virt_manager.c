#include "../include/virt_manager.h"

page_directory* current_page_directory;

// Get entry in page table for given address
pt_entry* get_pt_entry(page_table *pt, virtual_address address) {
    if (pt) return &pt->entries[PT_INDEX(address)];
    return 0;
}

// Get entry in page directory for given address
pd_entry* get_pd_entry(page_table *pd, virtual_address address) {
    if (pd) return &pd->entries[PT_INDEX(address)];
    return 0;
}

// Return a page for a given virtual address in the current
//   page directory
pt_entry* get_page(const virtual_address address) {
    // Get page directory
    page_directory *pd = current_page_directory; 

    // Get page table in directory
    pd_entry   *entry = &pd->entries[PD_INDEX(address)];
    page_table *table = (page_table *)PAGE_PHYS_ADDRESS(entry);

    // Get page in table
    pt_entry *page = &table->entries[PT_INDEX(address)];
    
    return page;
}

// Allocate a page of memory
void* allocate_page(pt_entry* page) {
    void* block = allocate_blocks(1);
    if (block != 0) {
        SET_FRAME(page, (physical_address)block);
        SET_ATTRIBUTE(page, PTE_PRESENT);
    } else kprintf("Page allocation error!\n");

    return block;
}

// Free a page of memory
void free_page(pt_entry *page) {
    void *address = (void*)PAGE_PHYS_ADDRESS(page);
    if (address) free_blocks(address, 1);

    CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

// Set the current page directory
bool set_page_directory(page_directory* pd) {
    if (!pd) return false;

    current_page_directory = pd;
    asm ("movl %%eax, %%cr3" : : "a"(current_page_directory) );

    return true;
}

// Flush a single page from the TLB (translation lookaside buffer)
void flush_tlb_entry(virtual_address address) {
    asm ("cli; invlpg (%0); sti" : : "r"(address) );
}

// Map a page
bool map_page(void *phys_address, void *virt_address) {
    page_directory *pd = current_page_directory;
    pd_entry *entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

    // TODO: Use TEST_ATTRIBUTE for this check?
    if ((*entry & PTE_PRESENT) != PTE_PRESENT) {
        // Page table not present allocate it
        page_table *table = (page_table *)allocate_blocks(1);
        if (!table) return false;   // Out of memory

        // Clear page table
        memset(table, 0, sizeof(page_table));

        // Create new entry
        pd_entry *entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

        // Map in the table & enable attributes
        SET_ATTRIBUTE(entry, PDE_PRESENT);
        SET_ATTRIBUTE(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (physical_address)table);
    }

    // Get table 
    page_table *table = (page_table *)PAGE_PHYS_ADDRESS(entry);
    pt_entry *page = &table->entries[PT_INDEX((uint32_t)virt_address)];

    // Map in page
    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_FRAME(page, (uint32_t)phys_address);

    return true;
}

// Unmap a page
void unmap_page(void *virt_address) {
    pt_entry *page = get_page((uint32_t)virt_address);

    SET_FRAME(page, 0); // Set physical address to 0 (effectively this is now a null pointer)
    CLEAR_ATTRIBUTE(page, PTE_PRESENT); // Set as not present, will trigger a #PF
}

void page_fault(Registers regs) {
	uint32_t faulting_address;
	asm ("mov %%cr2, %0" : "=r" (faulting_address));

	// The error code gives us detailing og what happened.
	int present	 = !(regs.error & 0x1);	    // When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
	int rw		 = regs.error & 0x2;		// When set, the page fault was caused by a write access. When not set, it was caused by a read access.
	int us		 = regs.error & 0x4;		// When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
	int reserved = regs.error & 0x8;		// When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
	int id		 = regs.error & 0x10;		// When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.

	// Output en error message.
	kprintf("Page fault! ( ");
	if (present) kprintf("present ");
	if (rw) kprintf("read-only ");
	if (us) kprintf("user-mode ");
	if (reserved) kprintf("reserved ");
	kprintf(") at 0x%u\n", faulting_address);

	kernel_panic("Page fault");
}

// Initialize virtual memory manager
bool initialize_virtual_memory_manager(uint32_t memory_start) {
    page_directory* dir = (page_directory*)allocate_blocks(3);
    memset(dir, 0, sizeof(page_directory));
    if (dir == NULL) return false;

    for (int i = 0; i < TABLES_PER_DIRECTORY; i++)
        dir->entries[i] = 0x02;

    page_table* table = (page_table*)allocate_blocks(1);
    memset(table, 0, sizeof(page_table));
    if (table == NULL) return false;

    page_table* table3G = (page_table*)allocate_blocks(1);
    memset(table3G, 0, sizeof(page_table));
    if (table3G == NULL) return false;

    for (uint32_t i = 0, frame = 0x0, virt = 0x0; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        table3G->entries[PT_INDEX(virt)] = page;
    }

    for (uint32_t i = 0, frame = memory_start, virt = 0xC0000000; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        pt_entry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, frame);

        table->entries[PT_INDEX(virt)] = page;
    }
    
    pd_entry* entry = &dir->entries[PD_INDEX(0xC0000000)];
    SET_ATTRIBUTE(entry, PDE_PRESENT);
    SET_ATTRIBUTE(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (uint32_t)table);

    pd_entry* second_entry = &dir->entries[PD_INDEX(0x00000000)];
    SET_ATTRIBUTE(second_entry, PDE_PRESENT);
    SET_ATTRIBUTE(second_entry, PDE_READ_WRITE);   
    SET_FRAME(second_entry, (physical_address)table3G); 

    if (set_page_directory(dir) == false) return false;
    asm ("movl %CR0, %EAX; orl $0x80000001, %EAX; movl %EAX, %CR0");
    
    i686_isr_registerHandler(14, page_fault);
    return true;
}