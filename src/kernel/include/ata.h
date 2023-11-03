#include <stdint.h>

#include "util.h"
#include "memory.h"
#include "string.h"

#define SECTOR_SIZE 512

bool isSectorEmpty(const char* sectorData, size_t sectorSize);

char* readSector(uint32_t LBA);
int writeSector(uint32_t lba, const uint8_t* buffer);

void appendSector(uint32_t lba, char* append_data);
void clearSector(uint32_t LBA);

int findEmptySector();
