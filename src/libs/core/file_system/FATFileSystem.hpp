#pragma once

#include "FileSystem.hpp"
#include "fat/FATData.hpp"

constexpr int FATRequiredMemory = 0x10000;

class FATFileSystem : public FileSystem {
    public:
        FATFileSystem();
        
        virtual bool Initialize(BlockDevice* device) override;

        virtual File* RootDirectory() override;

        bool ReadSector(uint32_t lba, uint8_t* buffer, size_t count = 1);
        bool ReadSectorFromCluster(uint32_t cluster, uint32_t sectorOffset, uint8_t* buffer);
        uint32_t GetNextCluster(uint32_t currentCluster);

        uint8_t FatType() const { return _FatType; }
        FATData& Data() { return *_Data; }

        FATFile* AllocateFile();
        void ReleaseFile(FATFile*);

        FATFileEntry* AllocateFileEntry();
        void ReleaseFileEntry(FATFileEntry*);

    private:
        bool ReadBootSector();
        uint32_t ClusterToLba(uint32_t cluster);
        void DetectFatType();
        bool ReadFat(uint32_t lbaOffset);

        BlockDevice* _Device;
        FATData* _Data;
        uint32_t _DataSectionLba;
        uint8_t _FatType;
        uint32_t _TotalSectors;
        uint32_t _SectorsPerFat;
};