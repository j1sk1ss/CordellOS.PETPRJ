#include "fat.h"
#include "stdio.h"
#include "memdefs.h"
#include "string.h"
#include "memory.h"
#include "ctype.h"
#include "math.h"
#include "stdlib.h"

#include <stddef.h>
#include <stdbool.h>

#define SECTOR_SIZE             512
#define MAX_PATH_SIZE           256
#define MAX_FILE_HANDLES        10
#define ROOT_DIRECTORY_HANDLE   -1
#define FAT_CASHE_SIZE          5

#pragma pack(push, 1)

typedef struct {
    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];

} __attribute__((packed)) FATExtendedBootRecord;

typedef struct  {
    uint32_t SectorsPerFat;
    uint16_t Flags;
    uint16_t FatVersion;
    uint32_t RootDirectoryClaster;
    uint16_t FSInfoSector;
    uint16_t BackupBootSector;
    uint8_t Reserved[12];

    FATExtendedBootRecord EBR;

} __attribute__((packed)) FAT32ExtendedBootRecord;


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

    union {
        FATExtendedBootRecord EBR12_16;
        FAT32ExtendedBootRecord EBR32;
    };

} __attribute__((packed))FAT_BootSector;

#pragma pack(pop)


typedef struct {
    uint8_t Buffer[SECTOR_SIZE];
    FAT_file Public;
    bool Opened;
    uint32_t FirstCluster;
    uint32_t CurrentCluster;
    uint32_t CurrentSectorInCluster;

} FAT_FileData;

typedef struct {
    uint8_t Order;
    int16_t Characters[12];
} FAT_LFNBlock;;


typedef struct {
    union {
        FAT_BootSector BootSector;
        uint8_t BootSectorBytes[SECTOR_SIZE];
    } BootSector;

    FAT_FileData RootDirectory;

    FAT_FileData OpenedFiles[MAX_FILE_HANDLES];

    uint8_t FatCashe[FAT_CASHE_SIZE * SECTOR_SIZE];
    uint32_t FatCashePosition;

    FAT_LFNBlock LFNBlocks[FAT_LFN_LAST];
    int LFNCount;

} FAT_Data;

static FAT_Data* _data;
static uint32_t _dataSectionLba;
static uint8_t _fatType;
static uint32_t _sectorsPerFat;
static uint32_t _totalSectors;

uint32_t FAT_clusterToLba(uint32_t cluster);

int FAT_CompareLFNBlocks(const void* firtsBlock, const void* secomdBlock) {
    FAT_LFNBlock* first = (FAT_LFNBlock*)firtsBlock;
    FAT_LFNBlock* second = (FAT_LFNBlock*)secomdBlock;

    return ((int)first->Order) - ((int)second->Order);
}

bool FAT_readBootSector(Partition* disk) {
    return Partition_readSectors(disk, 0, 1, _data->BootSector.BootSectorBytes);
}

bool FAT_readFat(Partition* disk, size_t lbaIndex) {
    return Partition_readSectors(disk, _data->BootSector.BootSector.ReservedSectors + lbaIndex, FAT_CASHE_SIZE, _data->FatCashe);
}

////////////////////////////////////////////////////////////////////////////// (..) //////////////
//  *BOOT SECTOR*   //      //         //           //                      // (..) //
// FST_STG  SEC_STG //      // *FAT 1* //   *FAT 2* //  *ROOT DIRECTORY*    // (..) //  *DATA*
//  *BOOT SECTOR*   //      //         //           //                      // (..) //
////////////////////////////////////////////////////////////////////////////// (..) //////////////

void FAT_detect(Partition* disk) {
    uint32_t dataClusters = (_totalSectors - _dataSectionLba) / _data->BootSector.BootSector.SectorsPerCluster;
    if (dataClusters < 0xFF5) 
        _fatType = 12;
    else if (_data->BootSector.BootSector.SectorsPerFat != 0) 
        _fatType = 16;
    else 
        _fatType = 32;
}

bool FAT_initialize(Partition* disk) {
    _data = (FAT_Data*)MEMORY_FAT_ADDR;

    // read boot sector
    if (!FAT_readBootSector(disk)) {
        printf("Cordell-FAT: read boot sector failed\r\n");
        return false;
    }

    _data->FatCashePosition = 0xFFFFFFFF;

    _totalSectors = _data->BootSector.BootSector.TotalSectors;
    if (_totalSectors == 0)             // fat32
        _totalSectors = _data->BootSector.BootSector.LargeSectorCount;

    bool isFat32 = false;
    _sectorsPerFat = _data->BootSector.BootSector.SectorsPerFat;
    if (_sectorsPerFat == 0)  {
        isFat32 = false;
        _sectorsPerFat = _data->BootSector.BootSector.EBR32.SectorsPerFat;
    }

    // open root directory file
    uint32_t rootDirLba;
    uint32_t rootDirSize;

    if (isFat32) {
        _dataSectionLba = _data->BootSector.BootSector.ReservedSectors + _sectorsPerFat *  _data->BootSector.BootSector.FatCount;
        rootDirLba = FAT_clusterToLba(_data->BootSector.BootSector.EBR32.RootDirectoryClaster);

        rootDirSize = 0;
    }
    else {
        rootDirLba  = _data->BootSector.BootSector.ReservedSectors + _sectorsPerFat * _data->BootSector.BootSector.FatCount;
        rootDirSize = sizeof(FAT_directoryEntry) * _data->BootSector.BootSector.DirEntryCount;

        uint32_t rootDirSectors = (rootDirSize + _data->BootSector.BootSector.BytesPerSector - 1) / _data->BootSector.BootSector.BytesPerSector;
        _dataSectionLba = rootDirLba + rootDirSectors;
    }


    _data->RootDirectory.Public.Handle          = ROOT_DIRECTORY_HANDLE;
    _data->RootDirectory.Public.IsDirectory     = true;
    _data->RootDirectory.Public.Position        = 0;
    _data->RootDirectory.Public.Size            = sizeof(FAT_directoryEntry) * _data->BootSector.BootSector.DirEntryCount;
    _data->RootDirectory.Opened                 = true;
    _data->RootDirectory.FirstCluster           = rootDirLba;
    _data->RootDirectory.CurrentCluster         = rootDirLba;
    _data->RootDirectory.CurrentSectorInCluster = 0;

    if (!Partition_readSectors(disk, rootDirLba, 1, _data->RootDirectory.Buffer)) {
        printf("Cordell-FAT: read root directory failed\r\n");
        return false;
    }

    FAT_detect(disk);

    // reset opened files
    for (int i = 0; i < MAX_FILE_HANDLES; i++)
        _data->OpenedFiles[i].Opened = false;

    _data->LFNCount = 9;

    return true;
}

uint32_t FAT_clusterToLba(uint32_t cluster) {
    return _dataSectionLba + (cluster - 2) * _data->BootSector.BootSector.SectorsPerCluster;
}

FAT_file* FAT_openEntry(Partition* disk, FAT_directoryEntry* entry) {
    // find empty handle
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++) 
        if (!_data->OpenedFiles[i].Opened)
            handle = i;
    
    // out of handles
    if (handle < 0) {
        printf("Cordell-FAT: out of file handles\r\n");
        return false;
    }

    // setup vars
    FAT_FileData* fileData           = &_data->OpenedFiles[handle];

    fileData->Public.Handle          = handle;
    fileData->Public.IsDirectory     = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fileData->Public.Position        = 0;
    fileData->Public.Size            = entry->Size;
    fileData->FirstCluster           = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
    fileData->CurrentCluster         = fileData->FirstCluster;
    fileData->CurrentSectorInCluster = 0;

    if (!Partition_readSectors(disk, FAT_clusterToLba(fileData->CurrentCluster), 1, fileData->Buffer)) {
        printf("Cordell-FAT: read error\r\n");
        return false;
    }

    fileData->Opened = true;
    return &fileData->Public;
}

uint32_t FAT_nextCluster(Partition* disk, uint32_t currentCluster) {    
    // (How much bytes) Which entry we need to read
    uint32_t fatIndex;
    if (_fatType == 12)
        fatIndex = currentCluster * 3 / 2;
    else if (_fatType == 16)
        fatIndex = currentCluster * 2;      // have 2 bytes
    else // if (_fatType == 32)
        fatIndex = currentCluster * 4;      // have 4 bytes in cluster

   // Generate index where cluster exist
   // Make sure cash has right number
   uint32_t fatIndexSector = fatIndex / SECTOR_SIZE;
   if (fatIndexSector < _data->FatCashePosition || fatIndexSector >= _data->FatCashePosition + FAT_CASHE_SIZE) {
        FAT_readFat(disk, fatIndexSector);
        _data->FatCashePosition = fatIndexSector;
   }

   fatIndex -= (_data->FatCashePosition * SECTOR_SIZE); // Position in cash that was readed

   uint32_t nextCluster;
    if (_fatType == 12) {
        if (currentCluster % 2 == 0)
            nextCluster = (*(uint16_t*)(_data->FatCashe + fatIndex)) & 0x0FFF;
        else
            nextCluster = (*(uint16_t*)(_data->FatCashe + fatIndex)) >> 4;

        if (nextCluster >= 0xFF8) 
            nextCluster |= 0xFFFFF000;                  // Add FF..FF for cluster cuz for any FAT(`) different
    }
    else if (_fatType == 16) {
        nextCluster = *(uint16_t*)(_data->FatCashe + fatIndex);
        if (nextCluster >= 0xFFF8)
            nextCluster |= 0xFFFF0000;
    }
    else // if (_fatType == 32)
        nextCluster = *(uint32_t*)(_data->FatCashe + fatIndex);

    return nextCluster;
}

uint32_t FAT_read(Partition* disk, FAT_file* file, uint32_t byteCount, void* dataOut) {
    // get file data
    FAT_FileData* fileData = (file->Handle == ROOT_DIRECTORY_HANDLE) 
        ? &_data->RootDirectory 
        : &_data->OpenedFiles[file->Handle];

    uint8_t* u8DataOut = (uint8_t*)dataOut;

    // don't read past the end of the file
    if (!fileData->Public.IsDirectory || (fileData->Public.IsDirectory && fileData->Public.Size != 0)) 
        byteCount = min(byteCount, fileData->Public.Size - fileData->Public.Position);

    while (byteCount > 0) {
        uint32_t leftInBuffer = SECTOR_SIZE - (fileData->Public.Position % SECTOR_SIZE);
        uint32_t take = min(byteCount, leftInBuffer);

        memcpy(u8DataOut, fileData->Buffer + fileData->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fileData->Public.Position += take;
        byteCount -= take;

        // See if we need to read more data
        if (leftInBuffer == take) {
            // Special handling for root directory
            if (fileData->Public.Handle == ROOT_DIRECTORY_HANDLE) {
                ++fileData->CurrentCluster;

                // read next sector
                if (!Partition_readSectors(disk, fileData->CurrentCluster, 1, fileData->Buffer)) {
                    printf("Cordell-FAT: read error!\r\n");
                    break;
                }
            }
            else {
                // calculate next cluster & sector to read
                if (++fileData->CurrentSectorInCluster >= _data->BootSector.BootSector.SectorsPerCluster) {
                    fileData->CurrentSectorInCluster = 0;
                    fileData->CurrentCluster = FAT_nextCluster(disk, fileData->CurrentCluster);
                }

                if (fileData->CurrentCluster >= 0xFFFFFFF8) {
                    // Mark end of file
                    fileData->Public.Size = fileData->Public.Position;
                    break;
                }

                // read next sector
                if (!Partition_readSectors(disk, FAT_clusterToLba(fileData->CurrentCluster) + fileData->CurrentSectorInCluster, 1, fileData->Buffer)) {
                    printf("Cordell-FAT: read error!\r\n");
                    break;
                }
            }
        }
    }

    return u8DataOut - (uint8_t*)dataOut;
}

bool FAT_readEntry(Partition* disk, FAT_file* file, FAT_directoryEntry* dirEntry) {
    return FAT_read(disk, file, sizeof(FAT_directoryEntry), dirEntry) == sizeof(FAT_directoryEntry);
}

void FAT_close(FAT_file* file) {
    printf("Cordell-FAT is closing...\r\n");

    if (file->Handle == ROOT_DIRECTORY_HANDLE) {
        file->Position = 0;
        _data->RootDirectory.CurrentCluster = _data->RootDirectory.FirstCluster;
    }
    else {
        _data->OpenedFiles[file->Handle].Opened = false;
    }

    printf("Cordell-FAT closed\r\n");
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

bool FAT_findFile(Partition* disk, FAT_file* file, const char* name, FAT_directoryEntry* entryOut) {
    char shortName[12];
    char longName[256];

    FAT_directoryEntry entry;

    FAT_getShortName(name, shortName);

    while (FAT_readEntry(disk, file, &entry)) {
        // if (entry.Attributes == FAT_ATTRIBUTE_LFN) {
        //     FAT_longFileEntry* lfn = (FAT_longFileEntry*)&entry;

        //     int index = _data->LFNCount++;
        //     _data->LFNBlocks[index] = lfn->Order & (FAT_LFN_LAST - 1);

        //     memcpy(_data->LFNBlocks[index].Characters, lfn->CharsFirst, sizeof(lfn->CharsFirst));
        //     memcpy(_data->LFNBlocks[index].Characters + 5, lfn->CharsSecond, sizeof(lfn->CharsSecond));
        //     memcpy(_data->LFNBlocks[index].Characters + 11, lfn->CharsThird, sizeof(lfn->CharsThird));

        //     // is the last LFN character / block
        //     if ((lfn->Order & FAT_LFN_LAST) != 0) {
        //         qsort(_data->LFNBlocks, _data->LFNCount, sizeof(FAT_LFNBlock), FAT_CompareLFNBlocks);
        //         char* namePosition = longName;

        //         for (int i = 0; i < _data->LFNCount; i++) {
        //             int16_t* chars      = _data->LFNBlocks[i].Characters;
        //             int16_t* charsLimit = chars + 13;

        //             while (chars < charsLimit && *chars != 0) {
        //                 int codePoint;

        //                 chars           = utf16_to_codepoint(chars, &codePoint);
        //                 namePosition    = codepoint_to_utf8(codePoint, namePosition);
        //             }
        //         }

        //         *namePosition = 0;
        //     }
        // }

        if (memcmp(shortName, entry.Name, 11) == 0) {
            *entryOut = entry;
            return true;
        }
    }
    
    return false;
}

FAT_file* FAT_open(Partition* disk, const char* path) {
    printf("Starting opening Cordell-FAT: ");
    printf(path);
    printf("\r\n");

    char name[MAX_PATH_SIZE];

    // ignore leading slash
    if (path[0] == '/')
        path++;

    FAT_file* current = &_data->RootDirectory.Public;

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
        FAT_directoryEntry entry;
        if (FAT_findFile(disk, current, name, &entry)) {
            FAT_close(current);

            // check if directory
            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0) {
                printf("Cordell-FAT: %s not a directory\r\n", name);
                return NULL;
            }

            // open new directory entry
            current = FAT_openEntry(disk, &entry);
        }
        else {
            FAT_close(current);

            printf("Cordell-FAT: %s not found\r\n", name);
            return NULL;
        }
    }

    printf("\r\n");
    printf("Opening ended succes!\r\n");
    return current;
}
