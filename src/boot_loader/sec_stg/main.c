#include <stdint.h>
#include <boot/bootparams.h>

#include "x86/x86.h"
#include "x86/vbe.h"

#include "std/stdio.h"
#include "std/stdlib.h"

#include "file_system/elf.h"
#include "file_system/mbr.h"
#include "file_system/disk.h"
#include "file_system/fat.h"

#include "memory/memdefs.h"
#include "memory/memory.h"
#include "memory/memdetect.h"

uint8_t* KernelLoadBuffer   = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel             = (uint8_t*)MEMORY_KERNEL_ADDR;

BootParams _bootParams;

typedef void (*KernelStart)(BootParams* bootParams, void* partition, DISK* disk, Partition* partirion);

void __attribute__((cdecl)) start(uint16_t bootDrive, void* partition) {
    clrscr();

    DISK disk;
    if (!DISK_initialize(&disk, bootDrive)) {
        printf("Cordell-Disk init error\r\n");
        goto end;
    }

    Partition part;
    MBR_detectPartition(&part, &disk, partition);

    if (!FAT_init(&part)) {
        printf("Cordell-FAT init error\r\n");
        goto end;
    }

    // prepare boot params
    _bootParams.BootDevice = bootDrive;
    memory_detect(&_bootParams.Memory);

    // load kernel
    KernelStart kernelEntry;
    if (!ELF_Read(&part, "/boot/kernel.elf", (void**)&kernelEntry)) {
        printf("ELF read failed, booting halted");
        goto end;
    }

    // execute kernel
    kernelEntry(&_bootParams, &partition, &disk, &disk);

end:
    for(;;);
}