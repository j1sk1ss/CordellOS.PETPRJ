#include <core/Defs.hpp>
#include <core/arch/i686/VGATextDevice.hpp>
#include <core/arch/i686/E9Device.hpp>
#include <core/devices/TextDevice.hpp>
#include <core/devices/RangeBlockDevice.hpp>
#include <core/Debug.hpp>
#include <core/file_system/FATFileSystem.hpp>

#include <arch/i686/BIOSDisk.hpp>
#include <arch/i686/RealMemory.hpp>

#include <devices/MBR.hpp>

#include <mem/SecStageAllocator.hpp>

#include <cpp/NewDelete.hpp>

#include <stdint.h>
#include <memdefs.h>

arch::i686::VGATextDevice _VGADevice;
arch::i686::E9Device _debugDevice;
SecStageAllocator _allocator(reinterpret_cast<void*>(MEMORY_MIN), MEMORY_MAX - MEMORY_MIN);

EXPORT void ASMCALL Start(uint16_t bootDrive, uint32_t partition) {
    SetCppAllocator(&_allocator);

    _VGADevice.Clear();
    
    TextDevice screen(&_VGADevice);
    Debug::AddOutputDevice(&screen, Debug::Level::Info, false);
    TextDevice debug(&_debugDevice);
    Debug::AddOutputDevice(&debug, Debug::Level::Debug, true);

    BIOSDisk disk(bootDrive);
    if (!disk.Initialize()) 
        Debug::Critical("second stage", "Failed to initialize disk!");

    // Handle partitioned disks
    BlockDevice* part;
    RangeBlockDevice partRange;
    if (bootDrive < 0x80)  
        part = &disk;
    else {
        MBREntry* entry = ToLinear<MBREntry*>(partition);
        partRange.Initialize(&disk, entry->LbaStart, entry->Size);
        part = &partRange;
    }

    Debug::Info("second stage", "Start FAT");

    // Read partition
    FATFileSystem fs;
    if (!fs.Initialize(part)) 
        Debug::Critical("second stage", "Failed to initialize FAT file system!");

    File* kernel = fs.Open("kernel.elf", FileOpenMode::Read);

    Debug::Info("second stage", "OK!");

    for (;;);
}