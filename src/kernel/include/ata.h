#include <stdint.h>

#include "util.h"
#include "memory.h"

void readSector(uint32_t LBA, char* buffer);
void writeSector(uint32_t lba, const uint8_t* buffer);
void clearSector(uint32_t LBA);