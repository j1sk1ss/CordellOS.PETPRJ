#pragma once

#include "malloc.h"

#include <stddef.h>

void free(void *ptr);
meta_ptr merge_blocks(meta_ptr block);
meta_ptr get_block_addr(void *p);
int is_addr_valid(void *p);
void free(void *ptr);