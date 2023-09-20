#include "mbr.h"

#include "../memory/memory.h"

#include "../std/stdio.h"

typedef struct  {
    uint8_t Attributes;
    uint8_t ChsStart[3];
    uint8_t PartitionType;
    uint8_t ChsEnd[3];
    uint32_t LbaStart;
    uint32_t Size;
} __attribute__((packed)) MBREntry;


uint32_t MBR_detectPartition(Partition* part, DISK* disk, void* partition) {
    part->Disk = disk;

    if (disk->id < 0x80) {
        part->PartitionOffset = 0;

        part->PartitionSize = 
                (uint32_t)(disk->cylinders) * 
                (uint32_t)(disk->heads) * 
                (uint32_t)(disk->sectors);
    }
    else {
        MBREntry* entry = (MBREntry*)seg_offset_to_linear(partition);

        part->PartitionOffset = entry->LbaStart;
        part->PartitionSize = entry->Size;
    }
}

bool Partition_ReadSectors(Partition* partition, uint32_t lba, uint8_t sectors, void* lowerDataOut) {
    return DISK_readSectors(partition->Disk, lba + partition->PartitionOffset, sectors, lowerDataOut);
}

bool Partition_WriteSectors(Partition* partition, uint32_t lba, uint8_t sectors, const void* dataIn) {
    return DISK_writeSectors(partition->Disk, lba + partition->PartitionOffset, sectors, dataIn);
}