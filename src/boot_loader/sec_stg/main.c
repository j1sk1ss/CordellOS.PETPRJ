#include <stdint.h>

#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "vbe.h"

uint8_t* KernelLoadBuffer   = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel             = (uint8_t*)MEMORY_KERNEL_ADDR;

#define COLOR(r, g, b)        ((b) | (g << 8) | (r << 16))

typedef void (*KernelStart)();

void __attribute__((cdecl)) start(uint16_t bootDrive) {
    clrscr();

    DISK disk;
    if (!DISK_Initialize(&disk, bootDrive)) {
        printf("Disk init error\r\n");
        goto end;
    }

    if (!FAT_initialize(&disk)) {
        printf("FAT init error\r\n");
        goto end;
    }

    // load kernel
    FAT_file* fatFile = FAT_open(&disk, "/kernel.bin");
    uint32_t read;
    uint8_t* kernelBuffer = Kernel;

    while ((read = FAT_read(&disk, fatFile, MEMORY_LOAD_SIZE, KernelLoadBuffer))) {
        memcpy(kernelBuffer, KernelLoadBuffer, read);
        kernelBuffer += read;
    }

    FAT_close(fatFile);

    const int desiredWidht   = 1024;    // Widght of monitor
    const int desiredHeight  = 768;     // Height of monitor
    const int desiredBpp     = 32;      // Bytes per pixel of monitor

    uint16_t pickedMode = 0xFFFF;

    // Initialize graphics
    VbeInfoBlock* info              = (VbeInfoBlock*)MEMORY_VESA_INFO;
    VBEModeInfoStructure* modeInfo  = (VBEModeInfoStructure*)MEMORY_MODE_INFO;

    if (vbe_getControllerInfo(info)) {
        printf("VBE success");

        uint16_t* mode = (uint16_t*)info->VideoModePtr;
        for (int i = 0; mode[i] != 0xFFFF; i++) {
            if (!vbe_getModeInfo(mode[i], modeInfo)) {
                printf("Can't get mode info %x\n\n", mode[i]);
                continue;
            }

            bool hasFB = (modeInfo->attributes & 0x90) == 0x90;
            if (hasFB && modeInfo->width == desiredWidht && modeInfo->height == desiredHeight && modeInfo->bpp == desiredBpp) {
                pickedMode = mode[i];     
                printf("Found mode: %x\n", mode[i]);
                break;
            }
        }
    } else {
        printf("VBE not success");
    }

    // execute kernel
    KernelStart kernelStart = (KernelStart)Kernel;
    kernelStart();

    end:
        for(;;);
}