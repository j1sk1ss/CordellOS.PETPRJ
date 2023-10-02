#include "BIOSDisk.hpp"
#include "RealMemory.hpp"

#include <core/Defs.hpp>
#include <core/Assert.hpp>
#include <core/cpp/Algorithm.hpp>
#include <core/Memory.hpp>

EXPORT bool ASMCALL i686_Disk_GetDriveParams(uint8_t drive,
                                             uint8_t* driveTypeOut,
                                             uint16_t* cylindersOut,
                                             uint16_t* sectorsOut,
                                             uint16_t* headsOut);

EXPORT bool ASMCALL i686_Disk_Reset(uint8_t drive);

EXPORT bool ASMCALL i686_Disk_Read(uint8_t drive,
                                   uint16_t cylinder,
                                   uint16_t sector,
                                   uint16_t head,
                                   uint8_t count,
                                   void* lowerDataOut);

EXPORT bool ASMCALL i686_Disk_ExtensionsPresent(uint8_t drive);

struct ExtendedReadParameters {
    uint8_t ParamsSize;
    uint8_t Reserved;

    uint16_t Count;

    uint32_t Buffer;

    uint64_t LBA;       // Level Block Accesses
} __attribute__((packed));

EXPORT bool ASMCALL i686_Disk_ExtendedRead(uint8_t drive,
                                           ExtendedReadParameters* params);

struct ExtendedDriveParameters {
    uint16_t ParamsSize;
    uint16_t Flags;

    uint32_t Cylinders;
    uint32_t Heads;
    uint32_t SectorsPerTrack;
    
    uint64_t Sectors;
    
    uint16_t BytesPerSector;
} __attribute__((packed));

EXPORT bool ASMCALL i686_Disk_ExtendedGetDriveParams(uint8_t drive,
                                                     ExtendedDriveParameters* params);

BIOSDisk::BIOSDisk(uint8_t id): _Id(id), _Pos(-1), _Size(0) { }

bool BIOSDisk::Initialize() {
    _HaveExtensions = i686_Disk_ExtensionsPresent(_Id);

    if (_HaveExtensions) {
        ExtendedDriveParameters params;
        params.ParamsSize = sizeof(ExtendedDriveParameters);

        if (!i686_Disk_ExtendedGetDriveParams(_Id, &params))
            return false;

        Assert(params.BytesPerSector == SectorSize);
        _Size = SectorSize * params.Sectors;
    }
    else {
        uint8_t driveType;
        if (!i686_Disk_GetDriveParams(_Id, &driveType, &_Cylinders, &_Sectors, &_Heads))
            return false;
    }

    return true;
}

size_t BIOSDisk::Read(uint8_t* data, size_t size) {
    size_t initialPos = _Pos;
    if (_Pos == -1) {
        ReadNextSector();
        _Pos = 0;
    }

    if (_Pos >= _Size)
        return 0;
    
    while (size > 0) {
        size_t bufferPos    = _Pos % SectorSize;
        size_t canRead      = Min(size, SectorSize - bufferPos);

        Memory::Copy(data, _Buffer + bufferPos, canRead);

        size    -= canRead;
        data    += canRead;
        _Pos   += canRead;

        if (size > 0) 
            ReadNextSector();
    }

    return _Pos - initialPos;
}

bool BIOSDisk::ReadNextSector() {
    uint64_t lba    = _Pos / SectorSize;
    bool ok         = false;

    if (_HaveExtensions) {
        ExtendedReadParameters params;

        params.ParamsSize   = sizeof(ExtendedDriveParameters);
        params.Reserved     = 0;
        params.Count        = 1;
        params.Buffer       = ToSegOffset(_Buffer);
        params.LBA          = lba;

        for (int i = 0; i < 3 && !ok; i++)  {
            ok = i686_Disk_ExtendedRead(_Id, &params);
            if (!ok)
                i686_Disk_Reset(_Id);
        }
    }
    else {
        uint16_t cylinder, sector, head;
        LbaToChs(lba, &cylinder, &sector, &head);

        for (int i = 0; i < 3 && !ok; i++)  {
            ok = i686_Disk_Read(_Id, cylinder, sector, head, 1, _Buffer);
            if (!ok)
                i686_Disk_Reset(_Id);
        }
    }

    return ok;
}

size_t BIOSDisk::Print(const uint8_t* data, size_t size) {
    return 0;
}

void BIOSDisk::LbaToChs(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut) {
    *sectorOut      = lba % _Sectors + 1;              // sector = (LBA % sectors per track + 1)
    *cylinderOut    = (lba / _Sectors) / _Heads;      // cylinder = (LBA / sectors per track) / heads
    *headOut        = (lba / _Sectors) % _Heads;      // head = (LBA / sectors per track) % heads
}

bool BIOSDisk::Seek(SeekPosition pos, int rel) {
    switch (pos) {
        case SeekPosition::StartPosition:
            _Pos = -1;
            return true;

        case SeekPosition::CurrentPosition:
            _Pos += rel;
            ReadNextSector();
            return true;

        case SeekPosition::EndPosition:
            _Pos = _Size;
            return true;
    }

    return false;
}

size_t BIOSDisk::Size() {
    return _Size;
}

size_t BIOSDisk::Position() {
    return _Pos;
}