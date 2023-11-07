#include "../../include/fat.h"
#include "../../include/stdio.h"
#include "../../include/string.h"
#include "../../include/memory.h"

#include "../../../libs/core/shared/include/math.h"
#include "../../../libs/core/shared/include/memdefs.h"

#include <stddef.h>

// Constants
#define SECTOR_SIZE             512
#define MAX_PATH_SIZE           256
#define MAX_FILE_HANDLES        10
#define ROOT_DIRECTORY_HANDLE   -1

// Structure to represent the Boot Sector of a FAT file system
typedef struct {
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
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

    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];

} __attribute__((packed)) FAT_BootSector;

// Structure to represent data of a file in the FAT file system
typedef struct {
    uint8_t Buffer[SECTOR_SIZE];
    FAT_File Public;
    bool Opened;
    uint32_t FirstCluster;
    uint32_t CurrentCluster;
    uint32_t CurrentSectorInCluster;

} FAT_FileData;

// Structure to represent overall data of the FAT file system
typedef struct {
    union {
        FAT_BootSector BootSector;
        uint8_t BootSectorBytes[SECTOR_SIZE];
    } BS;

    FAT_FileData RootDirectory;

    FAT_FileData OpenedFiles[MAX_FILE_HANDLES];

} FAT_Data;

// Global variables
static FAT_Data* _data;
static uint8_t* g_Fat = NULL;
static uint32_t _dataSectionLba;

// Function to read the boot sector of the FAT file system
bool FAT_read_boot_sector(DISK* disk) {
    return DISK_read_sectors(disk, 0, 1, _data->BS.BootSectorBytes);
}

// Function to read the File Allocation Table (FAT) of the FAT file system
bool FAT_read_fat(DISK* disk) {
    return DISK_read_sectors(disk, _data->BS.BootSector.ReservedSectors, _data->BS.BootSector.SectorsPerFat, g_Fat);
}

// Function to initialize the FAT file system
bool FAT_initialize(DISK* disk) {
    _data = (FAT_Data*)MEMORY_FAT_ADDR;

    // read boot sector
    if (!FAT_read_boot_sector(disk)) {
        printf("FAT: read boot sector failed\r\n");
        return false;
    }

    // read FAT
    g_Fat = (uint8_t*)_data + sizeof(FAT_Data);
    uint32_t fatSize = _data->BS.BootSector.BytesPerSector * _data->BS.BootSector.SectorsPerFat;
    if (sizeof(FAT_Data) + fatSize >= MEMORY_FAT_SIZE) {
        printf("FAT: not enough memory to read FAT! Required %lu, only have %u\r\n", sizeof(FAT_Data) + fatSize, MEMORY_FAT_SIZE);
        return false;
    }

    if (!FAT_read_fat(disk)) {
        printf("FAT: read FAT failed\r\n");
        return false;
    }

    // open root directory file
    uint32_t rootDirLba = _data->BS.BootSector.ReservedSectors + _data->BS.BootSector.SectorsPerFat * _data->BS.BootSector.FatCount;
    uint32_t rootDirSize = sizeof(FAT_DirectoryEntry) * _data->BS.BootSector.DirEntryCount;

    _data->RootDirectory.Public.Handle              = ROOT_DIRECTORY_HANDLE;
    _data->RootDirectory.Public.IsDirectory         = true;
    _data->RootDirectory.Public.Position            = 0;
    _data->RootDirectory.Public.Size                = sizeof(FAT_DirectoryEntry) * _data->BS.BootSector.DirEntryCount;
    _data->RootDirectory.Opened                     = true;
    _data->RootDirectory.FirstCluster               = rootDirLba;
    _data->RootDirectory.CurrentCluster             = rootDirLba;
    _data->RootDirectory.CurrentSectorInCluster     = 0;

    if (!DISK_read_sectors(disk, rootDirLba, 1, _data->RootDirectory.Buffer))  {
        printf("FAT: read root directory failed\r\n");
        return false;
    }

    // calculate data section
    uint32_t rootDirSectors = (rootDirSize + _data->BS.BootSector.BytesPerSector - 1) / _data->BS.BootSector.BytesPerSector;
    _dataSectionLba = rootDirLba + rootDirSectors;

    // reset opened files
    for (int i = 0; i < MAX_FILE_HANDLES; i++)
        _data->OpenedFiles[i].Opened = false;

    return true;
}

// Function to convert a cluster number to the corresponding Logical Block Address (LBA)
uint32_t FAT_cluster_to_lba(uint32_t cluster) {
    return _dataSectionLba + (cluster - 2) * _data->BS.BootSector.SectorsPerCluster;
}

// Function to open a file or directory entry in the FAT file system
FAT_File* FAT_open_entry(DISK* disk, FAT_DirectoryEntry* entry) {
    // find empty handle
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++) 
        if (!_data->OpenedFiles[i].Opened)
            handle = i;

    // out of handles
    if (handle < 0) {
        printf("FAT: out of file handles\r\n");
        return false;
    }

    // setup vars
    FAT_FileData* fd            = &_data->OpenedFiles[handle];
    fd->Public.Handle           = handle;
    fd->Public.IsDirectory      = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position         = 0;
    fd->Public.Size             = entry->Size;
    fd->FirstCluster            = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
    fd->CurrentCluster          = fd->FirstCluster;
    fd->CurrentSectorInCluster  = 0;

    if (!DISK_read_sectors(disk, FAT_cluster_to_lba(fd->CurrentCluster), 1, fd->Buffer)) {
        printf("FAT: open entry failed - read error cluster=%u lba=%u\n", fd->CurrentCluster, FAT_cluster_to_lba(fd->CurrentCluster));
        for (int i = 0; i < 11; i++)
            printf("%c", entry->Name[i]);
            
        printf("\n");
        return false;
    }

    fd->Opened = true;
    return &fd->Public;
}

// Function to get the next cluster in the FAT
uint32_t FAT_next_cluster(uint32_t currentCluster) {    
    uint32_t fatIndex = currentCluster * 3 / 2;

    if (currentCluster % 2 == 0)
        return (*(uint16_t*)(g_Fat + fatIndex)) & 0x0FFF;
    else
        return (*(uint16_t*)(g_Fat + fatIndex)) >> 4;
}

// Function to read data from a file in the FAT file system
uint32_t FAT_read(DISK* disk, FAT_File* file, uint32_t byteCount, void* dataOut) {
    // get file data
    FAT_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) 
        ? &_data->RootDirectory 
        : &_data->OpenedFiles[file->Handle];

    uint8_t* u8DataOut = (uint8_t*)dataOut;

    // don't read past the end of the file
    if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0))
        byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);

    while (byteCount > 0) {
        uint32_t leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        uint32_t take = min(byteCount, leftInBuffer);

        memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->Public.Position += take;
        byteCount -= take;

        // See if we need to read more data
        if (leftInBuffer == take) {
            // Special handling for root directory
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE) {
                ++fd->CurrentCluster;

                // read next sector
                if (!DISK_read_sectors(disk, fd->CurrentCluster, 1, fd->Buffer)) {
                    printf("FAT: read error!\r\n");
                    break;
                }
            }
            else {
                // calculate next cluster & sector to read
                if (++fd->CurrentSectorInCluster >= _data->BS.BootSector.SectorsPerCluster) {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = FAT_next_cluster(fd->CurrentCluster);
                }

                if (fd->CurrentCluster >= 0xFF8) {
                    // Mark end of file
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                // read next sector
                if (!DISK_read_sectors(disk, FAT_cluster_to_lba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer)) {
                    printf("FAT: read error!\r\n");
                    break;
                }
            }
        }
    }

    return u8DataOut - (uint8_t*)dataOut;
}

// Function to read a directory entry from the FAT file system
bool FAT_read_entry(DISK* disk, FAT_File* file, FAT_DirectoryEntry* dirEntry) {
    return FAT_read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry) == sizeof(FAT_DirectoryEntry);
}

// Function to close a file or directory in the FAT file system
void FAT_close(FAT_File* file) {
    if (file->Handle == ROOT_DIRECTORY_HANDLE) {
        file->Position = 0;
        _data->RootDirectory.CurrentCluster = _data->RootDirectory.FirstCluster;
    }
    else {
        _data->OpenedFiles[file->Handle].Opened = false;
    }
}

// Function to find a file in the FAT file system
bool FAT_find_file(DISK* disk, FAT_File* file, const char* name, FAT_DirectoryEntry* entryOut) {
    char fatName[12];
    FAT_DirectoryEntry entry;

    // convert from name to fat name
    memset(fatName, ' ', sizeof(fatName));
    fatName[11] = '\0';

    const char* ext = strchr(name, '.');
    if (ext == NULL)
        ext = name + 11;

    // for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
    //     fatName[i] = toupper(name[i]);

    if (ext != name + 11) {
        for (int i = 0; i < 3 && ext[i + 1]; i++)
            fatName[i + 8] = toupper(ext[i + 1]);
    }

    while (FAT_read_entry(disk, file, &entry)) {
        if (memcmp(fatName, entry.Name, 11) == 0) {
            *entryOut = entry;
            return true;
        }        
    }
    
    return false;
}

// Function to open a file or directory in the FAT file system
FAT_File* FAT_open(DISK* disk, const char* path) {
    char name[MAX_PATH_SIZE];

    // ignore leading slash
    if (path[0] == '/')
        path++;

    FAT_File* current = &_data->RootDirectory.Public;

    while (*path) {
        // extract next file name from path
        bool isLast = false;
        const char* delim = strchr(path, '/');
        if (delim != NULL) {
            memcpy(name, path, delim - path);
            name[delim - path + 1] = '\0';
            path = delim + 1;
        }
        else {
            unsigned len = strlen(path);
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = true;
        }
        
        // find directory entry in current directory
        FAT_DirectoryEntry entry;
        if (FAT_find_file(disk, current, name, &entry)) {
            FAT_close(current);

            // check if directory
            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0) {
                printf("FAT: %s not a directory\r\n", name);
                return NULL;
            }

            // open new directory entry
            current = FAT_open_entry(disk, &entry);
        }
        else {
            FAT_close(current);

            printf("FAT: %s not found\r\n", name);
            return NULL;
        }
    }

    return current;
}