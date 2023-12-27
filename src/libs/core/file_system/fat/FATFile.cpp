#include "FATFile.hpp"

#include <file_system/FATFileSystem.hpp>
#include <core/Debug.hpp>
#include <core/cpp/Algorithm.hpp>
#include <core/Memory.hpp>

#define LOG_MODULE "FatFile"

FATFile::FATFile()
    : _FS(nullptr),
      _Opened(false),
      _IsRootDirectory(false),
      _Position(),
      _Size(),
      _FirstCluster(),
      _CurrentCluster(),
      _CurrentClusterIndex(0),
      _CurrentSectorInCluster() {}

bool FATFile::Open(FATFileSystem* fs, uint32_t firstCluster, const char* name, uint32_t size, bool isDirectory) {
    _IsRootDirectory           = false;
    _Position                  = 0;
    _Size                      = size;
    _IsDirectory               = isDirectory;
    _FirstCluster              = firstCluster;
    _CurrentCluster            = _FirstCluster;
    _CurrentClusterIndex       = 0;
    _CurrentSectorInCluster    = 0;

    if (!_FS->ReadSectorFromCluster(_CurrentCluster, _CurrentSectorInCluster, _Buffer)) {
        Debug::Error(LOG_MODULE, "FAT: open file %s failed - read error cluster=%u\n", name, _CurrentCluster);
        return false;
    }

    _Opened = true;
    return true;
}

bool FATFile::OpenRootDirectory1216(FATFileSystem* fs, uint32_t rootDirLba, uint32_t rootDirSize) {
    _IsRootDirectory           = true;
    _Position                  = 0;
    _Size                      = rootDirSize;
    _FirstCluster              = rootDirLba;
    _CurrentCluster            = _FirstCluster;
    _CurrentClusterIndex       = 0;
    _CurrentSectorInCluster    = 0;

    if (!_FS->ReadSector(rootDirLba, _Buffer)) {
        Debug::Error(LOG_MODULE, "FAT: read root directory failed\r\n");
        return false;
    }

    return true;
}

void FATFile::Release() {
    _FS->ReleaseFile(this);
}

bool FATFile::ReadFileEntry(FATDirectoryEntry* dirEntry) {
    return Read(reinterpret_cast<uint8_t*>(dirEntry), sizeof(FATDirectoryEntry)) == sizeof(FATDirectoryEntry);
}

size_t FATFile::Read(uint8_t* data, size_t byteCount) {
    uint8_t* originalData = data;

    // don't read past the end of the file
    if (!_IsDirectory || (_IsDirectory && _Size != 0))
        byteCount = Min(byteCount, _Size - _Position);

    while (byteCount > 0) {
        uint32_t leftInBuffer = SectorSize - (_Position % SectorSize);
        uint32_t take = Min(byteCount, leftInBuffer);

        Memory::Copy(data, _Buffer + _Position % SectorSize, take);

        data       += take;
        _Position  += take;
        byteCount  -= take;

        if (leftInBuffer == take) {
            if (_IsRootDirectory) {
                ++_CurrentCluster;

                // read next sector
                if (!_FS->ReadSector(_CurrentCluster, _Buffer)) {
                    Debug::Error(LOG_MODULE, "FAT: read error!\r\n");
                    break;
                }
            }
            else {
                // calculate next cluster & sector to read
                if (++_CurrentSectorInCluster >= _FS->Data().BS.BootSector.SectorsPerCluster) {
                    _CurrentSectorInCluster = 0;
                    _CurrentCluster = _FS->GetNextCluster(_CurrentCluster);
                    ++_CurrentClusterIndex;
                }

                if (_CurrentCluster >= 0xFFFFFFF8) {
                    // Mark end of file
                    _Size = _Position;
                    break;
                }

                // read next sector
                if (!_FS->ReadSectorFromCluster(_CurrentCluster, _CurrentSectorInCluster, _Buffer)) {
                    Debug::Error(LOG_MODULE, "FAT: read error!");
                    break;
                }
            }
        }
    }

    return data - originalData;
}

size_t FATFile::Print(const uint8_t* data, size_t size) {
    // not supported (yet)
    return 0;
}

bool FATFile::Seek(SeekPosition pos, int rel) {
    switch (pos) {
        case SeekPosition::StartPosition:
            _Position = static_cast<uint32_t>(Max(0, rel));
            break;
        
        case SeekPosition::CurrentPosition:
            if (rel < 0 && _Position < -rel)
                _Position = 0;

            _Position = Min(Size(), static_cast<uint32_t>(_Position + rel));
            
        case SeekPosition::EndPosition:
            if (rel < 0 && Size() < -rel)
                _Position = 0;
            _Position = Min(Size(), static_cast<uint32_t>(Size() + rel));

            break;
    }

    UpdateCurrentCluster();
    return true;
}

bool FATFile::UpdateCurrentCluster() {
    uint32_t clusterSize    = _FS->Data().BS.BootSector.SectorsPerCluster * SectorSize;
    uint32_t desiredCluster = _Position / clusterSize;
    uint32_t desiredSector  = (_Position % clusterSize) / SectorSize;
    
    if (desiredCluster == _CurrentClusterIndex && desiredSector == _CurrentSectorInCluster)
        return true;

    if (desiredCluster < _CurrentClusterIndex) {
        _CurrentClusterIndex   = 0;
        _CurrentCluster        = _FirstCluster;
    }

    while (desiredCluster > _CurrentClusterIndex) {
        _CurrentCluster = _FS->GetNextCluster(_CurrentCluster);
        ++_CurrentClusterIndex;
    }

    _CurrentSectorInCluster = desiredSector;
    return _FS->ReadSectorFromCluster(_CurrentCluster, _CurrentSectorInCluster, _Buffer);
}

FileEntry* FATFile::ReadFileEntry() {
    FATDirectoryEntry entry;
    if (ReadFileEntry(&entry)) {
        FATFileEntry* fileEntry = _FS->AllocateFileEntry();
        fileEntry->Initialize(_FS, entry);
        return fileEntry;
    }

    return nullptr;
}