#pragma once

#include "../File.hpp"
#include "../FileEntry.hpp"
#include "../fat/FATHeaders.hpp"
#include "../fat/FATFileEntry.hpp"

class FATFileSystem;

class FATFile : public File {
    public:
        FATFile();
        bool Open(FATFileSystem* fs, uint32_t firstCluster, const char* name, uint32_t size, bool isDirectory);
        bool OpenRootDirectory1216(FATFileSystem* fs, uint32_t rootDirLba, uint32_t rootDirSize);

        virtual void Release() override;

        bool IsOpened() const { return _Opened; }
        bool ReadFileEntry(FATDirectoryEntry* dirEntry);

        virtual size_t Read(uint8_t* data, size_t size) override;
        virtual size_t Print(const uint8_t* data, size_t size) override;
        virtual bool Seek(SeekPosition pos, int rel) override;
        virtual size_t Size() override { return _Size; }
        virtual size_t Position() override { return _Position; }
        
        virtual FileEntry* ReadFileEntry() override;
        

    private:
        bool UpdateCurrentCluster();

        FATFileSystem* _FS;
        uint8_t _Buffer[SectorSize];

        bool _Opened;
        bool _IsRootDirectory;
        bool _IsDirectory;
        
        uint32_t _Position;
        uint32_t _Size;
        uint32_t _FirstCluster;
        uint32_t _CurrentCluster;
        uint32_t _CurrentClusterIndex;
        uint32_t _CurrentSectorInCluster; 
};