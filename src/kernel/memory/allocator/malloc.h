#include <stdint.h>

#include "../memory.h"
#include "../../io/stdio.h"
#include "../../arch/i686/isr.h"


typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void mm_init(uint32_t kernel_end);
void free(void *mem);
void pfree(void *mem);
char* pmalloc(size_t size);
void* malloc(size_t size);