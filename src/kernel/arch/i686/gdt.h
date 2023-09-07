#pragma once

#define i686_GDT_CODE_SEGMENT 0x08
#define i686_GDT_DATA_SEGMENT 0x10

void __attribute__((cdecl)) i686_gdt_initialize();