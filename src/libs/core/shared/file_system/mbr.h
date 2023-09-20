#pragma once

#include "disk.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    DISK* Disk;
    uint32_t PartitionOffset;
    uint32_t PartitionSize;
} Partition;



uint32_t MBR_detectPartition(Partition* part, DISK* disk, void* partition);
bool Partition_ReadSectors(Partition* partition, uint32_t lba, uint8_t sectors, void* lowerDataOut);
bool Partition_WriteSectors(Partition* partition, uint32_t lba, uint8_t sectors, const void* dataIn);