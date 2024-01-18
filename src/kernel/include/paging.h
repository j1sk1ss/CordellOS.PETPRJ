#include "memory.h"
#include "stdio.h"
#include "x86.h"

void paging_init(uint32_t kernel_end);

void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);
void paging_enable();