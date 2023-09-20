#pragma once

#include <stdint.h>

#include "mbr.h"

#include <stdbool.h>

typedef struct {
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) FAT_DirectoryEntry;

#pragma pack(pop)

typedef struct {
    uint8_t Order;
    int16_t CharsFirst[5];
    uint8_t Attribute;
    uint8_t LongEntryType;
    uint8_t Checksum;
    int16_t CharsSecond[6];
    uint16_t AlwaysZero;
    int16_t CharsThird[2];
} __attribute__((packed)) FAT_longFileEntry;

#define FAT_LFN_LAST        0x40

typedef struct {
    int Handle;
    bool IsDirectory;
    uint32_t Position;
    uint32_t Size;
} __attribute__((packed)) FAT_File;

enum FAT_Attributes {
    FAT_ATTRIBUTE_READ_ONLY         = 0x01,
    FAT_ATTRIBUTE_HIDDEN            = 0x02,
    FAT_ATTRIBUTE_SYSTEM            = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID         = 0x08,
    FAT_ATTRIBUTE_DIRECTORY         = 0x10,
    FAT_ATTRIBUTE_ARCHIVE           = 0x20,
    FAT_ATTRIBUTE_LFN               = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

bool FAT_init(Partition* disk);
FAT_File* FAT_open(Partition* disk, const char* path);
uint32_t FAT_read(Partition* disk, FAT_File* file, uint32_t byteCount, void* dataOut);
bool FAT_readEntry(Partition* disk, FAT_File* file, FAT_DirectoryEntry* dirEntry);
void FAT_close(FAT_File* file);
