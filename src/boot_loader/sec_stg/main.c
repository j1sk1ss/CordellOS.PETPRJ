#include <stdint.h>

#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "vbe.h"
#include "mbr.h"

uint8_t* KernelLoadBuffer   = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel             = (uint8_t*)MEMORY_KERNEL_ADDR;

typedef void (*KernelStart)();

void __attribute__((cdecl)) start(uint16_t bootDrive, void* partition) {
    clrscr();

    printf("partition=%x\n", partition);

    DISK disk;
    if (!DISK_initialize(&disk, bootDrive)) {
        printf("Cordell-Disk init error\r\n");
        goto end;
    }

    Partition part;
    MBR_detectPartition(&part, &disk, partition);

    if (!FAT_initialize(&part)) {
        printf("Cordell-FAT init error\r\n");
        goto end;
    }

    // load kernel
    FAT_file* fatFile = FAT_open(&part, "/boot/kernel.elf");
    uint32_t read;
    uint8_t* kernelBuffer = Kernel;

    while ((read = FAT_read(&part, fatFile, MEMORY_LOAD_SIZE, KernelLoadBuffer))) {
        memcpy(kernelBuffer, KernelLoadBuffer, read);
        kernelBuffer += read;
    }

    FAT_close(fatFile);

    // execute kernel
    KernelStart kernelStart = (KernelStart)Kernel;
    kernelStart();

    end:
        for(;;);
}