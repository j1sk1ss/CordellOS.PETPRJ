#pragma once
#include <stdint.h>

bool __attribute__((cdecl)) i386_disk_read(uint8_t drive, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void* lowerDataOut);
void __attribute__((cdecl)) i386_disk_reset(uint8_t drive);