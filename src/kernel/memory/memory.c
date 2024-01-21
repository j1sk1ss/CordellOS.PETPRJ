#include "../include/memory.h"

page_directory_t *kernel_directory;
page_directory_t *current_directory;

void* memcpy(void* destination, const void* source, uint16_t num) {
    uint8_t* u8Dst = (uint8_t *)destination;
    const uint8_t* u8Src = (const uint8_t *)source;

    for (uint16_t i = 0; i < num; i++)
        u8Dst[i] = u8Src[i];

    return destination;
}

void* memset(void* pointer, int value, uint16_t num) {
    uint8_t* u8Ptr = (uint8_t *)pointer;

    for (uint16_t i = 0; i < num; i++)
        u8Ptr[i] = (uint8_t)value;

    return pointer;
}

int memcmp(const void* firstPointer, const void* secondPointer, uint16_t num) {
    const uint8_t* u8Ptr1 = (const uint8_t *)firstPointer;
    const uint8_t* u8Ptr2 = (const uint8_t *)secondPointer;

    for (uint16_t i = 0; i < num; i++)
        if (u8Ptr1[i] != u8Ptr2[i])
            return 1;

    return 0;
}

void* seg_offset_to_linear(void* address) {
    uint32_t offset = (uint32_t)(address) & 0xFFFF;
    uint32_t segment = (uint32_t)(address) >> 16;

    return (void*)(segment * 16 + offset);
}

void* memmove(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    if (d < s)
        while (len--)
            *d++ = *s++;
    else {
      char *lasts = s + (len-1);
      char *lastd = d + (len-1);
        while (len--)
            *lastd-- = *lasts--;
    }

    return dest;
}

uint32_t *frames;
uint32_t nframes;

void paging_install(uint32_t memsize) {
	// nframes = memsize  / 4;
	// frames  = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes * 8));
	// memset(frames, 0, INDEX_FROM_BIT(nframes));

	// uintptr_t phys;
	// kernel_directory = (page_directory_t *)kvmalloc_p(sizeof(page_directory_t),&phys);
	// memset(kernel_directory, 0, sizeof(page_directory_t));

	// uint32_t i = 0;
	// while (i < placement_pointer + 0x3000) {
	// 	alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
	// 	i += 0x1000;
	// }
	// /* XXX VGA TEXT MODE VIDEO MEMORY EXTENSION */
	// for (uint32_t j = 0xb8000; j < 0xc0000; j += 0x1000) {
	// 	alloc_frame(get_page(j, 1, kernel_directory), 0, 1);
	// }
	// isrs_install_handler(14, page_fault);
	// kernel_directory->physical_address = (uintptr_t)kernel_directory->physical_tables;

	// /* Kernel Heap Space */
	// for (i = placement_pointer; i < KERNEL_HEAP_END; i += 0x1000) {
	// 	alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
	// }

	// current_directory = clone_directory(kernel_directory);
	// switch_page_directory(kernel_directory);
}

uint32_t first_frame() {
    uint32_t i, j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		if (frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; ++j) {
				uint32_t testFrame = 0x1 << j;
				if (!(frames[i] & testFrame)) {
					return i * 0x20 + j;
				}
			}
		}
	}

	kprintf("\033[1;37;41mWARNING: System claims to be out of usable memory, which means we probably overwrote the page frames.\033[0m\n");
	return -1;
}

void set_frame(uintptr_t frame_addr) {
	uint32_t frame  = frame_addr / 0x1000;
	uint32_t index  = INDEX_FROM_BIT(frame);
	uint32_t offset = OFFSET_FROM_BIT(frame);
	frames[index] |= (0x1 << offset);
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable ) {
	// if (page->frame != 0) {
	// 	page->present = 1;
	// 	page->rw      = (is_writeable == 1) ? 1 : 0;
	// 	page->user    = (is_kernel == 1)    ? 0 : 1;
	// 	return;
	// } else {
	// 	uint32_t index = first_frame();
	// 	assert(index != (uint32_t)-1 && "Out of frames.");
	// 	set_frame(index * 0x1000);
	// 	page->present = 1;
	// 	page->rw      = (is_writeable == 1) ? 1 : 0;
	// 	page->user    = (is_kernel == 1)    ? 0 : 1;
	// 	page->frame   = index;
	// }
}