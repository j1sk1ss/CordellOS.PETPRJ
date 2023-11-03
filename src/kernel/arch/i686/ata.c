#include "../../include/ata.h"

// Function to read a sector from the disk.
char* readSector(uint32_t LBA) {
    char* buffer = (char*)malloc(SECTOR_SIZE);
    if (buffer == NULL) 
        return NULL;

    outportb(0x1F6, 0xE0 | (0 << 4) | ((LBA >> 24) & 0x0F));
    outportb(0x1F1, 0x00);
    outportb(0x1F2, 1);
    outportb(0x1F3, (uint8_t)(LBA & 0xFF));
    outportb(0x1F4, (uint8_t)((LBA >> 8) & 0xFF));
    outportb(0x1F5, (uint8_t)((LBA >> 16) & 0xFF));
    outportb(0x1F7, 0x20);

    while ((inportb(0x1F7) & 0x8) == 0) 
        continue;

    for (int n = 0; n < 256; n++) {
        uint16_t value = inportw(0x1F0);
        buffer[n * 2] = value & 0xFF;
        buffer[n * 2 + 1] = value >> 8;
    }

    return buffer;
}

// Function to write a sector to the disk.
int writeSector(uint32_t lba, const uint8_t* buffer) {
    outportb(0x1F6, 0xE0 | (0 << 4) | ((lba >> 24) & 0x0F));
    outportb(0x1F1, 0x00);
    outportb(0x1F2, 1);
    outportb(0x1F3, (uint8_t)lba);
    outportb(0x1F4, (uint8_t)(lba >> 8));
    outportb(0x1F5, (uint8_t)(lba >> 16));
    outportb(0x1F7, 0x30);

    while ((inportb(0x1F7) & 0x8) == 0) 
        continue;
    
    // Write the sector data from the buffer.
    for (int i = 0; i < 256; i++) {
        uint16_t data = *((uint16_t*)(buffer + i * 2));
        outportw(0x1F0, data);
    }

    return 1;
}

// Function that add data to sector
void appendSector(uint32_t lba, char* append_data) {
    char* previous_data = readSector(lba);

    strcat(previous_data, append_data);
    writeSector(lba, previous_data);
}

// Function that clear sector
void clearSector(uint32_t LBA) {
    char buffer[512];  // Assuming 512-byte sectors

    // Fill the buffer with zeros
    memset(buffer, 0, sizeof(buffer));

    // Write the buffer to the specified sector
    writeSector(LBA, buffer);
}

// Function to check if a sector is empty (all bytes are zero)
bool isSectorEmpty(const char* sectorData, size_t sectorSize) {
    for (size_t i = 0; i < sectorSize; i++) 
        if (sectorData[i] != 0) 
            return false;
        
    return true;
}

#define SECTOR_SIZE     512
#define SECTOR_COUNT    1000

// Function to find an empty sector on the disk
int findEmptySector() {
    for (uint32_t sector = 0; sector < SECTOR_COUNT; sector++) {
        if (isSectorEmpty(readSector(sector), SECTOR_SIZE)) 
            return sector;
    }

    // Return -1 if no empty sector is found
    return -1;
}
