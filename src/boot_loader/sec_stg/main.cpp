#include <stdint.h>

#include <core/Defs.hpp>
#include <core/arch/i686/VGATextDevice.hpp>
#include <core/arch/i686/E9Device.hpp>
#include <arch/i686/BIOSDisk.hpp>
#include <core/devices/TextDevice.hpp>
#include <core/Debug.hpp>
#include <devices/MBR.hpp>
#include <arch/i686/RealMemory.hpp>
#include <core/devices/RangeBlockDevice.hpp>

arch::i686::VGATextDevice _vgaDevice;
arch::i686::E9Device      _debugDevice;

BlockDevice* 

EXPORT void ASMCALL Start(uint16_t bootDrive, void* partition) {

    _vgaDevice.Clear();

    TextDevice screen(&_vgaDevice);
    Debug::AddOutputDevice(&screen, Debug::Level::Debug, false);

    TextDevice debugScreen(&_debugDevice);
    Debug::AddOutputDevice(&debugScreen, Debug::Level::Debug, true);

    BIOSDisk disk(bootDrive);
    if (!disk.Initialize()) {
        Debug::Critical("second stage", "Fail in disk init!");
    }

    BlockDevice* partition;
    RangeBlockDevice partRange;
    if (bootDrive < 0x80) 
        partition = &disk;
    else {
        MBREntry* entry = ToLinear<MBREntry*>(partition);
        partRange.Initialize(&disk, entry->LbaStart, entry->Size);
        partition = &partRange;
    }

    Debug::Info("second stage", "Disk init");

    for (;;);

}