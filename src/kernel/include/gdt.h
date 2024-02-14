#pragma once

#define i386_GDT_CODE_SEGMENT 0x08
#define i386_GDT_DATA_SEGMENT 0x10

void __attribute__((cdecl)) i386_gdt_initialize();