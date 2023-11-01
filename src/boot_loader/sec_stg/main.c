#include <stdint.h>

#include <boot/bootparams.h>

#include "../libs/core/shared/include/x86.h"
#include "../libs/core/shared/include/vbe.h"
#include "../libs/core/shared/include/stdio.h"
#include "../libs/core/shared/include/stdlib.h"
#include "../libs/core/shared/include/elf.h"
#include "../libs/core/shared/include/mbr.h"
#include "../libs/core/shared/include/disk.h"
#include "../libs/core/shared/include/fat.h"
#include "../libs/core/shared/include/memdefs.h"
#include "../libs/core/shared/include/memory.h"
#include "../libs/core/shared/include/memdetect.h"

// Place in memory where will be situated kernel
uint8_t* KernelLoadBuffer   = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel             = (uint8_t*)MEMORY_KERNEL_ADDR;
// Place in memory where will be situated kernel

BootParams _bootParams;

typedef void (*KernelStart)(BootParams* bootParams);

void __attribute__((cdecl)) start(uint16_t bootDrive, void* partition) {
    clrscr();

    //////////////////
    //
    //  DISK INITIALIZATION

        DISK disk;
        if (!DISK_initialize(&disk, bootDrive)) {
            printf("[main.c 35] Disk init error\r\n");
            goto end;
        }

    //  DISK INITIALIZATION
    //
    //////////////////
    //
    // PARTITION DETECTION

        Partition part;
        MBR_detectPartition(&part, &disk, partition);

        if (!FAT_init(&part)) {
            printf("[main.c 49] FAT init error\r\n");
            goto end;
        }

    // PARTITION DETECTION
    //
    //////////////////
    //
    //  KERNEL LOADING

        // prepare boot params
        _bootParams.BootDevice = bootDrive;
        memory_detect(&_bootParams.Memory);

        // load kernel
        KernelStart kernelEntry;
        if (!ELF_Read(&part, "/boot/kernel.elf", (void**)&kernelEntry)) {
            printf("[main.c 66] ELF read failed, booting halted");
            goto end;
        }

        // execute kernel
        kernelEntry(&_bootParams);

    // KERNEL LOADING
    //
    //////////////////

end:
    for(;;);
}