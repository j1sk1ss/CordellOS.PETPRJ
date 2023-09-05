#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t bool;
#define true 1
#define false 1

typedef struct {
    uint8_t BootJumpInstruction[3];
    uint8_t OemIndentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectopPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // Extended boot data
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;

    uint8_t VolumeLabel[11];       // Serial number
    uint8_t SystemId[8];           // 11 bytes, padded with spaces
} __attribute__((packed)) BootSector;

typedef struct {
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeIenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) DirectoryEntry;

BootSector _bootSector;
uint8_t* _fat = NULL;
DirectoryEntry* _directoryEntry = NULL;
uint32_t _rootDirectoryEnd;

bool readBootSector(FILE* disk) {
    return fread(&_bootSector, sizeof(_bootSector), 1, disk) > 0;
}

bool readSectors(FILE* disk, uint32_t lba, uint32_t count, void* bufferOut) {
    bool ok = true;
    ok = ok && (fseek(disk, lba * _bootSector.BytesPerSector, SEEK_SET) == 0);
    ok = ok && (fread(bufferOut, _bootSector.BytesPerSector, count, disk) == count);

    return ok;
}

bool readFat(FILE* disk) {
    _fat = (uint8_t*) malloc(_bootSector.SectorsPerFat * _bootSector.BytesPerSector);
    return readSectors(disk, _bootSector.ReservedSectors, _bootSector.SectorsPerFat, _fat);
}

bool readRootDirectory(FILE* disk) {
    uint32_t lba = _bootSector.ReservedSectors + _bootSector.SectorsPerFat * _bootSector.FatCount;
    uint32_t size = sizeof(DirectoryEntry) * _bootSector.DirEntryCount;
    uint32_t sectors = (size / _bootSector.BytesPerSector);
    if (size % _bootSector.BytesPerSector > 0) 
        sectors++;

    _rootDirectoryEnd = lba + sectors;
    _directoryEntry = (DirectoryEntry*) malloc(sectors * _bootSector.BytesPerSector);
    return readSectors(disk, lba, sectors, _directoryEntry);
}

DirectoryEntry* findFile(const char* name) {
    for (uint32_t i = 0; i < _bootSector.DirEntryCount; i++) 
        if (memcmp(name, _directoryEntry[i].Name, 11) == 0)
            return &_directoryEntry[i];
    
    return NULL;
}

bool readFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* outputBuffer) {
    bool ok = true;
    uint16_t currentCluster = fileEntry->FirstClusterLow;

    do {
        uint32_t lba = _rootDirectoryEnd + (currentCluster - 2) * _bootSector.SectopPerCluster;
        ok = ok && readSectors(disk, lba, _bootSector.SectopPerCluster, outputBuffer);
        outputBuffer += _bootSector.SectopPerCluster * _bootSector.BytesPerSector;

        uint32_t fatIndex = currentCluster * 3 / 2;
        if (currentCluster % 2 == 0) currentCluster = (*(uint16_t*)(_fat + fatIndex)) & 0x0FFF;
        else currentCluster = (*(uint16_t*)(_fat + fatIndex)) >> 4;
    } while (ok && currentCluster < 0x0FF8);
    
    return ok;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Syntax %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if (!disk) {
        fprintf(stderr, "Cannot open disk image %s!", argv[1]);
    }

    if (!readBootSector(disk)){
        fprintf(stderr, "Can't read boot sector!\n");
        return -2;
    }

    if (!readFat(disk)){
        fprintf(stderr, "Can't read FAT!\n");

        free(_fat);
        return -3;
    }

    if (!readRootDirectory(disk)){
        fprintf(stderr, "Can't read Root directory!\n");

        free(_directoryEntry);
        free(_fat);
        return -3;
    }

    DirectoryEntry* fileEntry = findFile(argv[2]);
    if (!fileEntry) {
        fprintf(stderr, "Can't find file %s!\n", argv[2]);

        free(_directoryEntry);
        free(_fat);
        return -5;
    }

    uint8_t* buffer = (uint8_t*) malloc(fileEntry->Size + _bootSector.BytesPerSector);
    if (!readFile(fileEntry, disk, buffer)) {
        fprintf(stderr, "Can't read file %s!\n", argv[2]);

        free(_directoryEntry);
        free(_fat);

        free(buffer);
        return -6;
    }

    for (size_t i = 0; i < fileEntry->Size; i++) {
        if (isprint(buffer[i])) fputc(buffer[i], stdout);
        else printf("<%02x>", buffer[i]);
    }

    printf("\n");

    free(buffer);

    free(_fat);
    free(_directoryEntry);
    return 0;
}