#include <stdint.h>
#include <boot/bootparams.h>

#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "vbe.h"
#include "mbr.h"
#include "stdlib.h"
#include "elf.h"
#include "memdetect.h"

uint8_t* KernelLoadBuffer   = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel             = (uint8_t*)MEMORY_KERNEL_ADDR;

BootParams _bootParams;

typedef void (*KernelStart)(BootParams* bootParams);

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
    kernelEntry(&_bootParams);

end:
    for(;;);
}