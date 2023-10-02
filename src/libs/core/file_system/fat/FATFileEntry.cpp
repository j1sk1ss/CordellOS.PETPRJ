#include "FATFileEntry.hpp"

#include <file_system/fat/FATFile.hpp>
#include <file_system/FATFileSystem.hpp>
#include <core/Debug.hpp>

FATFileEntry::FATFileEntry() : _FS(), _DirEntry() {}

void FATFileEntry::Release() {
    _FS->ReleaseFileEntry(this);
}

void FATFileEntry::Initialize(FATFileSystem* fs, const FATDirectoryEntry& dirEntry) {
    _FS        = fs;
    _DirEntry  = dirEntry;
}

const char* FATFileEntry::Name() {
    return reinterpret_cast<const char*>(_DirEntry.Name);
}

const FileType FATFileEntry::Type() {
    if (_DirEntry.Attributes & FAT_ATTRIBUTE_DIRECTORY)
        return FileType::Directory;

    return FileType::File;
}

File* FATFileEntry::Open(FileOpenMode mode) {
    FATFile* file = _FS->AllocateFile();
    if (file == nullptr) {
        Debug::Error("FAT File Entry: 32", "FAT Open error!");
        return nullptr;
    }

    // setup vars
    uint32_t size           = _DirEntry.Size;
    uint32_t firstCluster   = _DirEntry.FirstClusterLow + ((uint32_t)_DirEntry.FirstClusterHigh << 16);

    if (!file->Open(_FS, firstCluster, Name(), size, _DirEntry.Attributes & FAT_ATTRIBUTE_DIRECTORY)) {
        _FS->ReleaseFile(file);
        return nullptr;
    }

    return file;
}