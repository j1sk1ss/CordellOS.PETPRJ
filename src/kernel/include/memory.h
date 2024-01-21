#pragma once

#include <stdint.h>
#include <stddef.h>

#define SEG(seg)                (seg >> 16)
#define OFF(off)                (off & 0xFFFF)
#define SEGOFF2LINE(segoff)     ((SEG(segoff) << 4) + OFF(segoff))

#define INDEX_FROM_BIT(b) (b / 0x20)
#define OFFSET_FROM_BIT(b) (b % 0x20)

typedef struct page {
	uint32_t present:1;
	uint32_t rw:1;
	uint32_t user:1;
	uint32_t accessed:1;
	uint32_t dirty:1;
	uint32_t unused:7;
	uint32_t frame:20;
} __attribute__((packed)) page_t;

typedef struct page_table {
	page_t pages[1024];
} page_table_t;

typedef struct page_directory {
	page_table_t *tables[1024];	/* 1024 pointers to page tables... */
	uintptr_t physical_tables[1024];	/* Physical addresses of the tables */
	uintptr_t physical_address;	/* The physical address of physical_tables */

	int32_t ref_count;
} page_directory_t;

void* memcpy(void* destination, const void* source, uint16_t num);
void* memset(void* pointer, int value, uint16_t num);
int memcmp(const void* firstPointer, const void* secondPointer, uint16_t num);
void* memmove(void *dest, const void *src, size_t len);

void* seg_offset_to_linear(void* address);