#include "FATFileSystem.hpp"

#include <file_system/fat/FATHeaders.hpp>
#include <file_system/fat/FATFileEntry.hpp>
#include <Debug.hpp>

#define LOG_MODULE "FAT"

FATFileSystem::FATFileSystem() : 
    _Device(),
    _Data(new FATData()),
    _DataSectionLba(),
    _FatType(),
    _TotalSectors(),
    _SectorsPerFat() {}

bool FATFileSystem::Initialize(BlockDevice* device) {
    _Device = device;

    if (!ReadBootSector()) { // ERROR!
        Debug::Error(LOG_MODULE, "FAT: read boot sector failed");
        return false;
    }

    _Data->FatCachePosition = 0xFFFFFFFF;

    _TotalSectors = _Data->BS.BootSector.TotalSectors;
    if (_TotalSectors == 0)           // fat32
        _TotalSectors = _Data->BS.BootSector.LargeSectorCount;
    
    bool isFat32 = false;
    _SectorsPerFat = _Data->BS.BootSector.SectorsPerFat;
    if (_SectorsPerFat == 0) {         // fat32
        isFat32 = true;
        _SectorsPerFat = _Data->BS.BootSector.EBR32.SectorsPerFat;
    }
    
    Debug::Info(LOG_MODULE, "FAT: Open root dir of file starts");

    // open root directory file
    uint32_t rootDirLba;
    uint32_t rootDirSize;
    if (isFat32) {
        _DataSectionLba = _Data->BS.BootSector.ReservedSectors + _SectorsPerFat * _Data->BS.BootSector.FatCount;
        if (!_Data->RootDirectory.Open(this, _Data->BS.BootSector.EBR32.RootDirectoryCluster, "", 0, true))
            return false;
    }
    else {
        rootDirLba = _Data->BS.BootSector.ReservedSectors + _SectorsPerFat * _Data->BS.BootSector.FatCount;
        rootDirSize = sizeof(FATDirectoryEntry) * _Data->BS.BootSector.DirEntryCount;
        uint32_t rootDirSectors = (rootDirSize + _Data->BS.BootSector.BytesPerSector - 1) / _Data->BS.BootSector.BytesPerSector;
        _DataSectionLba = rootDirLba + rootDirSectors;

        if (!_Data->RootDirectory.OpenRootDirectory1216(this, rootDirLba, rootDirSize))
            return false;
    }

    DetectFatType();

    _Data->LFNCount = 0;

    Debug::Info(LOG_MODULE, "FAT: init ended");

    return true;
}

FATFile* FATFileSystem::AllocateFile() {
    return _Data->OpenedFilePool.Allocate();
}

void FATFileSystem::ReleaseFile(FATFile* file) {
    _Data->OpenedFilePool.Free(file);
}

FATFileEntry* FATFileSystem::AllocateFileEntry() {
    return _Data->FileEntryPool.Allocate();
}

void FATFileSystem::ReleaseFileEntry(FATFileEntry* fileEntry) {
    _Data->FileEntryPool.Free(fileEntry);
}

File* FATFileSystem::RootDirectory() {
    return &_Data->RootDirectory;
}

bool FATFileSystem::ReadSector(uint32_t lba, uint8_t* buffer, size_t count) { // Here is a problem!
    _Device->Seek(SeekPosition::StartPosition, lba * SectorSize);
    return (_Device->Read(buffer, count * SectorSize) == count * SectorSize);
}

bool FATFileSystem::ReadSectorFromCluster(uint32_t cluster, uint32_t sectorOffset, uint8_t* buffer) {
    return ReadSector(ClusterToLba(cluster) + sectorOffset, buffer);
}

bool FATFileSystem::ReadBootSector() {
    return ReadSector(0, _Data->BS.BootSectorBytes);
}

uint32_t FATFileSystem::ClusterToLba(uint32_t cluster) {
    return _DataSectionLba + (cluster - 2) * _Data->BS.BootSector.SectorsPerCluster;
}

void FATFileSystem::DetectFatType() {
    uint32_t dataClusters = (_TotalSectors - _DataSectionLba) / _Data->BS.BootSector.SectorsPerCluster;
    if (dataClusters < 0xFF5) 
        _FatType = 12;
    else if (_Data->BS.BootSector.SectorsPerFat != 0)
        _FatType = 16;
    else _FatType = 32;
}

uint32_t FATFileSystem::GetNextCluster(uint32_t currentCluster) {    
    // Determine the byte offset of the entry we need to read
    uint32_t fatIndex;
    if (_FatType == 12) 
        fatIndex = currentCluster * 3 / 2;
    else if (_FatType == 16) 
        fatIndex = currentCluster * 2;
    else /*if (_FatType == 32)*/ 
        fatIndex = currentCluster * 4;

    // Make sure cache has the right number
    uint32_t fatIndexSector = fatIndex / SectorSize;
    if (fatIndexSector < _Data->FatCachePosition || fatIndexSector >= _Data->FatCachePosition + FatCacheSize) {
        ReadFat(fatIndexSector);
        _Data->FatCachePosition = fatIndexSector;
    }

    fatIndex -= (_Data->FatCachePosition * SectorSize);

    uint32_t nextCluster;
    if (_FatType == 12) {
        if (currentCluster % 2 == 0)
            nextCluster = (*(uint16_t*)(_Data->FatCache + fatIndex)) & 0x0FFF;
        else
            nextCluster = (*(uint16_t*)(_Data->FatCache + fatIndex)) >> 4;
        
        if (nextCluster >= 0xFF8) 
            nextCluster |= 0xFFFFF000;
    }
    else if (_FatType == 16) {
        nextCluster = *(uint16_t*)(_Data->FatCache + fatIndex);
        
        if (nextCluster >= 0xFFF8) 
            nextCluster |= 0xFFFF0000;
    }
    else /*if (_FatType == 32)*/ 
        nextCluster = *(uint32_t*)(_Data->FatCache + fatIndex);

    return nextCluster;
}

bool FATFileSystem::ReadFat(uint32_t lbaOffset) {
    return ReadSector(_Data->BS.BootSector.ReservedSectors + lbaOffset, _Data->FatCache, FatCacheSize);
}