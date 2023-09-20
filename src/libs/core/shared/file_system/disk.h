#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t id;
    uint16_t cylinders;
    uint16_t sectors;
    uint16_t heads;
} DISK;

bool DISK_initialize(DISK* disk, uint8_t driveNumber);
bool DISK_readSectors(DISK* disk, uint32_t lba, uint8_t sectors, void* lowerDataOut);
bool DISK_writeSectors(DISK* disk, uint32_t lba, uint8_t sectors, const void* dataIn);