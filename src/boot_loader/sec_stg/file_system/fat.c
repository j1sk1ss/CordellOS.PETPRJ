#include "../include/fat.h"
#include "../include/memdefs.h"
#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/ctype.h"
#include "../include/math.h"
#include "../include/stdlib.h"

#include <stddef.h>

#define SECTOR_SIZE             512
#define MAX_PATH_SIZE           256
#define MAX_FILE_HANDLES        10
#define ROOT_DIRECTORY_HANDLE   -1
#define FAT_CACHE_SIZE          5

typedef struct {
    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];
} __attribute__((packed)) FAT_ExtendedBootRecord;

typedef struct  {
    uint32_t SectorsPerFat;
    uint16_t Flags;
    uint16_t FatVersion;
    uint32_t RootDirectoryCluster;
    uint16_t FSInfoSector;
    uint16_t BackupBootSector;
    uint8_t _Reserved[12];
    FAT_ExtendedBootRecord EBR;

} __attribute((packed)) FAT32_ExtendedBootRecord;

typedef struct  {
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

    union {
        FAT_ExtendedBootRecord EBR1216;
        FAT32_ExtendedBootRecord EBR32;
    };

} __attribute__((packed)) FAT_BootSector;


typedef struct {
    uint8_t Buffer[SECTOR_SIZE];
    FAT_File Public;
    bool Opened;
    uint32_t FirstCluster;
    uint32_t CurrentCluster;
    uint32_t CurrentSectorInCluster;

} FAT_FileData;

typedef struct {
    uint8_t Order;
    int16_t Chars[13];
} FAT_LFNBlock;

typedef struct {
    union {
        FAT_BootSector BootSector;
        uint8_t BootSectorBytes[SECTOR_SIZE];
    } BS;

    FAT_FileData RootDirectory;

    FAT_FileData OpenedFiles[MAX_FILE_HANDLES];

    uint8_t FatCache[FAT_CACHE_SIZE * SECTOR_SIZE];
    uint32_t FatCachePosition;

    FAT_LFNBlock LFNBlocks[FAT_LFN_LAST];
    int LFNCount;

} FAT_Data;

static FAT_Data* _data;
static uint32_t _dataSectionLba;
static uint8_t _fatType;
static uint32_t _totalSectors;
static uint32_t sectorsPerFat;

uint32_t FAT_clusterToLba(uint32_t cluster);

int FAT_compareLFNBlocks(const void* blockA, const void* blockB) {
    FAT_LFNBlock* a = (FAT_LFNBlock*)blockA;
    FAT_LFNBlock* b = (FAT_LFNBlock*)blockB;
    return ((int)a->Order) - ((int)b->Order);
}

bool FAT_readBootSector(Partition* disk) {
    return Partition_ReadSectors(disk, 0, 1, _data->BS.BootSectorBytes);
}

bool FAT_readFat(Partition* disk, size_t lbaIndex) {
    return Partition_ReadSectors(disk, _data->BS.BootSector.ReservedSectors + lbaIndex, FAT_CACHE_SIZE, _data->FatCache);
}

////////////////////////////////////////////////////////////////////////////// (..) //////////////
//  *BOOT SECTOR*   //      //         //           //                      // (..) //
// FST_STG  SEC_STG //      // *FAT 1* //   *FAT 2* //  *ROOT DIRECTORY*    // (..) //  *DATA*
//  *BOOT SECTOR*   //      //         //           //                      // (..) //
////////////////////////////////////////////////////////////////////////////// (..) //////////////

void FAT_detect(Partition* disk) {
    uint32_t dataClusters = (_totalSectors - _dataSectionLba) / _data->BS.BootSector.SectorsPerCluster;
    if (dataClusters < 0xFF5) 
        _fatType = 12;
    else if (_data->BS.BootSector.SectorsPerFat != 0)
        _fatType = 16;
    else _fatType = 32;
}

bool FAT_init(Partition* disk) {
    _data = (FAT_Data*)MEMORY_FAT_ADDR;

    // read boot sector
    if (!FAT_readBootSector(disk)) {
        printf("FAT: read boot sector failed\r\n");
        return false;
    }

    // read FAT
    _data->FatCachePosition = 0xFFFFFFFF;

    _totalSectors = _data->BS.BootSector.TotalSectors;
    if (_totalSectors == 0) {          // fat32
        _totalSectors = _data->BS.BootSector.LargeSectorCount;
    }

    bool isFat32 = false;
    sectorsPerFat = _data->BS.BootSector.SectorsPerFat;
    if (sectorsPerFat == 0) {         // fat32
        isFat32 = true;
        sectorsPerFat = _data->BS.BootSector.EBR32.SectorsPerFat;
    }
    
    // open root directory file
    uint32_t rootDirLba;
    uint32_t rootDirSize;
    if (isFat32) {
        _dataSectionLba = _data->BS.BootSector.ReservedSectors + sectorsPerFat * _data->BS.BootSector.FatCount;
        rootDirLba = FAT_clusterToLba( _data->BS.BootSector.EBR32.RootDirectoryCluster);
        rootDirSize = 0;
    }
    else {
        rootDirLba = _data->BS.BootSector.ReservedSectors + sectorsPerFat * _data->BS.BootSector.FatCount;
        rootDirSize = sizeof(FAT_DirectoryEntry) * _data->BS.BootSector.DirEntryCount;
        uint32_t rootDirSectors = (rootDirSize + _data->BS.BootSector.BytesPerSector - 1) / _data->BS.BootSector.BytesPerSector;
        _dataSectionLba = rootDirLba + rootDirSectors;
    }

    _data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    _data->RootDirectory.Public.IsDirectory = true;
    _data->RootDirectory.Public.Position = 0;
    _data->RootDirectory.Public.Size = sizeof(FAT_DirectoryEntry) * _data->BS.BootSector.DirEntryCount;
    _data->RootDirectory.Opened = true;
    _data->RootDirectory.FirstCluster = rootDirLba;
    _data->RootDirectory.CurrentCluster = rootDirLba;
    _data->RootDirectory.CurrentSectorInCluster = 0;

    if (!Partition_ReadSectors(disk, rootDirLba, 1, _data->RootDirectory.Buffer)) {
        printf("FAT: read root directory failed\r\n");
        return false;
    }

    // calculate data section
    FAT_detect(disk);

    // reset opened files
    for (int i = 0; i < MAX_FILE_HANDLES; i++)
        _data->OpenedFiles[i].Opened = false;
    _data->LFNCount = 0;

    return true;
}

uint32_t FAT_clusterToLba(uint32_t cluster) {
    return _dataSectionLba + (cluster - 2) * _data->BS.BootSector.SectorsPerCluster;
}

FAT_File* FAT_openEntry(Partition* disk, FAT_DirectoryEntry* entry)
{
    // find empty handle
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++) {
        if (!_data->OpenedFiles[i].Opened)
            handle = i;
    }

    // out of handles
    if (handle < 0) {
        printf("FAT: out of file handles\r\n");
        return false;
    }

    // setup vars
    FAT_FileData* fd = &_data->OpenedFiles[handle];
    fd->Public.Handle = handle;
    fd->Public.IsDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = entry->Size;
    fd->FirstCluster = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0;

    if (!Partition_ReadSectors(disk, FAT_clusterToLba(fd->CurrentCluster), 1, fd->Buffer)) {
        printf("FAT: open entry failed - read error cluster=%u lba=%u\n", fd->CurrentCluster, FAT_clusterToLba(fd->CurrentCluster));
        for (int i = 0; i < 11; i++)
            printf("%c", entry->Name[i]);
        printf("\n");
        return false;
    }

    fd->Opened = true;
    return &fd->Public;
}

uint32_t FAT_nextCluster(Partition* disk, uint32_t currentCluster) {    
    // Determine the byte offset of the entry we need to read
    uint32_t fatIndex;
    if (_fatType == 12) 
        fatIndex = currentCluster * 3 / 2;
    else if (_fatType == 16) 
        fatIndex = currentCluster * 2;
    else /*if (_fatType == 32)*/ 
        fatIndex = currentCluster * 4;

    // Make sure cache has the right number
    uint32_t fatIndexSector = fatIndex / SECTOR_SIZE;
    if (fatIndexSector < _data->FatCachePosition 
        || fatIndexSector >= _data->FatCachePosition + FAT_CACHE_SIZE)
    {
        FAT_readFat(disk, fatIndexSector);
        _data->FatCachePosition = fatIndexSector;
    }

    fatIndex -= (_data->FatCachePosition * SECTOR_SIZE);

    uint32_t nextCluster;
    if (_fatType == 12) {
        if (currentCluster % 2 == 0)
            nextCluster = (*(uint16_t*)(_data->FatCache + fatIndex)) & 0x0FFF;
        else
            nextCluster = (*(uint16_t*)(_data->FatCache + fatIndex)) >> 4;
        
        if (nextCluster >= 0xFF8) 
            nextCluster |= 0xFFFFF000;
    }
    else if (_fatType == 16) {
        nextCluster = *(uint16_t*)(_data->FatCache + fatIndex);
        if (nextCluster >= 0xFFF8) 
            nextCluster |= 0xFFFF0000;
    }
    else /*if (_fatType == 32)*/ 
        nextCluster = *(uint32_t*)(_data->FatCache + fatIndex);

    return nextCluster;
}

uint32_t FAT_read(Partition* disk, FAT_File* file, uint32_t byteCount, void* dataOut) {
    // get file data
    FAT_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) 
        ? &_data->RootDirectory 
        : &_data->OpenedFiles[file->Handle];

    uint8_t* u8DataOut = (uint8_t*)dataOut;

    // don't read past the end of the file
    if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0))
        byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);

    while (byteCount > 0)
    {
        uint32_t leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        uint32_t take = min(byteCount, leftInBuffer);

        memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->Public.Position += take;
        byteCount -= take;

        // printf("leftInBuffer=%lu take=%lu\r\n", leftInBuffer, take);
        // See if we need to read more data
        if (leftInBuffer == take) {
            // Special handling for root directory
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE) {
                ++fd->CurrentCluster;

                // read next sector
                if (!Partition_ReadSectors(disk, fd->CurrentCluster, 1, fd->Buffer)) {
                    printf("FAT: read error!\r\n");
                    break;
                }
            }
            else {
                // calculate next cluster & sector to read
                if (++fd->CurrentSectorInCluster >= _data->BS.BootSector.SectorsPerCluster) {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = FAT_nextCluster(disk, fd->CurrentCluster);
                }

                if (fd->CurrentCluster >= 0xFFFFFFF8) {
                    // Mark end of file
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                // read next sector
                if (!Partition_ReadSectors(disk, FAT_clusterToLba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer)) {
                    printf("FAT: read error!\r\n");
                    break;
                }
            }
        }
    }

    return u8DataOut - (uint8_t*)dataOut;
}

bool FAT_readEntry(Partition* disk, FAT_File* file, FAT_DirectoryEntry* dirEntry) {
    return FAT_read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry) == sizeof(FAT_DirectoryEntry);
}

void FAT_close(FAT_File* file) {
    if (file->Handle == ROOT_DIRECTORY_HANDLE) {
        file->Position = 0;
        _data->RootDirectory.CurrentCluster = _data->RootDirectory.FirstCluster;
    }
    else 
        _data->OpenedFiles[file->Handle].Opened = false;
}

void FAT_getShortName(const char* name, char shortName[12]) {
    // convert from name to fat name
    memset(shortName, ' ', 12);
    shortName[11] = '\0';

    const char* ext = strchr(name, '.');
    if (ext == NULL)
        ext = name + 11;

    for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
        shortName[i] = toupper(name[i]);

    if (ext != name + 11) 
        for (int i = 0; i < 3 && ext[i + 1]; i++)
            shortName[i + 8] = toupper(ext[i + 1]);
}

bool FAT_findFile(Partition* disk, FAT_File* file, const char* name, FAT_DirectoryEntry* entryOut) {
    char shortName[12];
    //char longName[256];
    FAT_DirectoryEntry entry;

    FAT_getShortName(name, shortName);

    while (FAT_readEntry(disk, file, &entry)) {
        /*if (entry.Attributes == FAT_ATTRIBUTE_LFN) {
            FAT_LongFileEntry* lfn = (FAT_LongFileEntry*)&entry;

            int idx = _data->LFNCount++;
            _data->LFNBlocks[idx].Order = lfn->Order & (FAT_LFN_LAST - 1);
            memcpy(_data->LFNBlocks[idx].Chars, lfn->Chars1, sizeof(lfn->Chars1));
            memcpy(_data->LFNBlocks[idx].Chars + 5, lfn->Chars2, sizeof(lfn->Chars2));
            memcpy(_data->LFNBlocks[idx].Chars + 11, lfn->Chars1, sizeof(lfn->Chars3));

            // is this the last LFN block
            if ((lfn->Order & FAT_LFN_LAST) != 0) {
                qsort(_data->LFNBlocks, _data->LFNCount, sizeof(FAT_LFNBlock), FAT_compareLFNBlocks);
                char* namePos = longName;
                for (int i = 0; i < _data->LFNCount; i++)
                {
                    int16_t* chars = _data->LFNBlocks[i].Chars;
                    int16_t* charsLimit = chars + 13;

                    while (chars < charsLimit && *chars != 0)
                    {
                        int codepoint;
                        chars = utf16_to_codepoint(chars, &codepoint);
                        namePos = codepoint_to_utf8(codepoint, namePos);
                    }
                }
                *namePos = 0;
                printf("LFN: %s\n", longName);
            }
        }*/

        if (memcmp(shortName, entry.Name, 11) == 0) {
            *entryOut = entry;
            return true;
        }
    }
    
    return false;
}

FAT_File* FAT_open(Partition* disk, const char* path) {
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
            name[delim - path] = '\0';
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
        if (FAT_findFile(disk, current, name, &entry)) {
            FAT_close(current);

            // check if directory
            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0) {
                printf("FAT: %s not a directory\r\n", name);
                return NULL;
            }

            // open new directory entry
            current = FAT_openEntry(disk, &entry);
        }
        else {
            FAT_close(current);

            printf("FAT: %s not found\r\n", name);
            return NULL;
        }
    }

    return current;
}