#include "../include/virt_manager.h"


page_directory* current_page_directory;


pt_entry* get_pt_entry(page_table* pt, virtual_address address) {
    if (pt) return &pt->entries[PT_INDEX(address)];
    return 0;
}

pd_entry* get_pd_entry(page_table* pd, virtual_address address) {
    if (pd) return &pd->entries[PT_INDEX(address)];
    return 0;
}

pt_entry* get_page(const virtual_address address) {
    page_directory* pd = current_page_directory; 
    pd_entry* entry    = &pd->entries[PD_INDEX(address)];
    page_table* table  = (page_table*)PAGE_PHYS_ADDRESS(entry);
    pt_entry* page     = &table->entries[PT_INDEX(address)];
    
    return page;
}

void* allocate_page(pt_entry* page) {
    void* block = allocate_blocks(1);
    if (block != 0) {
        SET_FRAME(page, (physical_address)block);
        SET_ATTRIBUTE(page, PTE_PRESENT);
    } else kprintf("Page allocation error!\n");

    return block;
}

void free_page(pt_entry* page) {
    void* address = (void*)PAGE_PHYS_ADDRESS(page);
    if (address) free_blocks(address, 1);

    CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

bool set_page_directory(page_directory* pd) {
    if (!pd) return false;

    current_page_directory = pd;
    asm ("movl %%eax, %%cr3" : : "a"(current_page_directory) );

    return true;
}

void flush_tlb_entry(virtual_address address) {
    asm ("cli; invlpg (%0); sti" : : "r"(address) );
}

bool map_page(void* phys_address, void* virt_address) {
    page_directory* pd = current_page_directory;
    pd_entry* entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

    if ((*entry & PTE_PRESENT) != PTE_PRESENT) {
        page_table* table = (page_table *)allocate_blocks(1);
        if (!table) return false;

        memset(table, 0, sizeof(page_table));
        pd_entry* entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];

        SET_ATTRIBUTE(entry, PDE_PRESENT);
        SET_ATTRIBUTE(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (physical_address)table);
    }

    page_table* table = (page_table*)PAGE_PHYS_ADDRESS(entry);
    pt_entry* page    = &table->entries[PT_INDEX((uint32_t)virt_address)];

    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_FRAME(page, (uint32_t)phys_address);

    return true;
}

void unmap_page(void *virt_address) {
    pt_entry* page = get_page((uint32_t)virt_address);

    SET_FRAME(page, 0);
    CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

void page_fault(Registers* regs) {
	uint32_t faulting_address;
	asm ("mov %%cr2, %0" : "=r" (faulting_address));

	int present	 = !(regs->error & 0x1);	// When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
	int rw		 = regs->error & 0x2;		// When set, the page fault was caused by a write access. When not set, it was caused by a read access.
	int us		 = regs->error & 0x4;		// When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
	int reserved = regs->error & 0x8;		// When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
	int id		 = regs->error & 0x10;		// When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.

	kprintf("Page fault! ( ");
	if (present) kprintf("present ");
	if (rw) kprintf("read-only ");
	if (us) kprintf("user-mode ");
	if (reserved) kprintf("reserved ");
	kprintf(") at 0x%u\n", faulting_address);

	kernel_panic("Page fault");
}

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
    i386_isr_registerHandler(14, page_fault);
    return true;
}

physical_address virtual2physical(void* virt_address) {
    page_directory* pd = current_page_directory;
    pd_entry* pd_entry = &pd->entries[PD_INDEX((uint32_t)virt_address)];
    if ((*pd_entry & PTE_PRESENT) != PTE_PRESENT) 
        return 0;

    page_table* pt     = (page_table*)PAGE_PHYS_ADDRESS(pd_entry);
    pt_entry* pt_entry = &pt->entries[PT_INDEX((uint32_t)virt_address)];
    if ((*pt_entry & PTE_PRESENT) != PTE_PRESENT) 
        return 0;

    physical_address phys_address = PAGE_PHYS_ADDRESS(pt_entry) | OFFSET_IN_PAGE((uint32_t)virt_address);
    return phys_address;
}

void print_page_map(char arg) {
    int free = 0;
    int occupied = 0;

    kprintf("\n");

    page_directory* pd = current_page_directory;
    for (int pd_index = 0; pd_index < TABLES_PER_DIRECTORY; pd_index++) {
        pd_entry* pd_entry = &pd->entries[pd_index];

        if ((*pd_entry & PDE_PRESENT) == PDE_PRESENT) {
            page_table* pt = (page_table*)PAGE_PHYS_ADDRESS(pd_entry);

            for (int pt_index = 0; pt_index < PAGES_PER_TABLE; pt_index++) {
                pt_entry* page = &pt->entries[pt_index];

                if (*page & PTE_PRESENT) occupied++;
                else free++;
                
                if (arg != 'c') kprintf("addr: %u [%c]\n", PAGE_PHYS_ADDRESS(page), (*page & PTE_PRESENT) ? 'O' : 'F');
            }
        }
    }

    kprintf("FREE: [%i] | OCCUP: [%i]\n", free, occupied);
    kprintf("FREE: [%iB] | OCCUP: [%iB]\n", free * PAGE_SIZE, occupied * PAGE_SIZE);
}