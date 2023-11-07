#pragma once
#include <stdint.h>
#include "disk.h"

// Structure defining a FAT directory entry
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

// Structure representing a file in the FAT file system
typedef struct  {
    int Handle;
    bool IsDirectory;
    uint32_t Position;
    uint32_t Size;
} FAT_File;

// Enum defining FAT file attributes
enum FAT_Attributes {
    FAT_ATTRIBUTE_READ_ONLY         = 0x01,
    FAT_ATTRIBUTE_HIDDEN            = 0x02,
    FAT_ATTRIBUTE_SYSTEM            = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID         = 0x08,
    FAT_ATTRIBUTE_DIRECTORY         = 0x10,
    FAT_ATTRIBUTE_ARCHIVE           = 0x20,
    FAT_ATTRIBUTE_LFN               = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

// Function to initialize the FAT file system on a disk
bool FAT_initialize(DISK* disk);

// Function to open a file or directory in the FAT file system
FAT_File * FAT_open(DISK* disk, const char* path);

// Function to read data from a file in the FAT file system
uint32_t FAT_read(DISK* disk, FAT_File* file, uint32_t byteCount, void* dataOut);

// Function to read a directory entry from a file in the FAT file system
bool FAT_read_entry(DISK* disk, FAT_File* file, FAT_DirectoryEntry* dirEntry);

// Function to close a file in the FAT file system
void FAT_close(FAT_File* file);