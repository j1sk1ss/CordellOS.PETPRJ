#include "../../include/ata.h"

// Function to read a sector from the disk.
void readSector(uint32_t LBA, char* buffer) {
    uint8_t slavebit = 0;
    uint8_t sectorCount = 1;

    outportb(0x1F6, 0xE0 | (slavebit << 4) | ((LBA >> 24) & 0x0F));
    outportb(0x1F1, 0x00);
    outportb(0x1F2, sectorCount);
    outportb(0x1F3, (uint8_t)(LBA & 0xFF));
    outportb(0x1F4, (uint8_t)((LBA >> 8) & 0xFF));
    outportb(0x1F5, (uint8_t)((LBA >> 16) & 0xFF));
    outportb(0x1F7, 0x20);

    while ((inportb(0x1F7) & 0x8) == 0) continue;

    for (int n = 0; n < 256; n++) {
        uint16_t value = inportw(0x1F0);
        buffer[n * 2] = value & 0xFF;
        buffer[n * 2 + 1] = value >> 8;
    }
}

// Function to write a sector to the disk.
void writeSector(uint32_t lba, const uint8_t* buffer) {
    outportb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outportb(0x1F1, 0x00);
    outportb(0x1F2, 1);
    outportb(0x1F3, (uint8_t)lba);
    outportb(0x1F4, (uint8_t)(lba >> 8));
    outportb(0x1F5, (uint8_t)(lba >> 16));
    outportb(0x1F7, 0x30);

    while ((inportb(0x1F7) & 0x8) == 0) continue;

    // Write the sector data from the buffer.
    for (int i = 0; i < 256; i++) {
        uint16_t data = *((uint16_t*)(buffer + i * 2));
        outportw(0x1F0, data);
    }
}

void clearSector(uint32_t LBA) {
    char buffer[512];  // Assuming 512-byte sectors

    // Fill the buffer with zeros
    memset(buffer, 0, sizeof(buffer));

    // Write the buffer to the specified sector
    writeSector(LBA, buffer);
}
