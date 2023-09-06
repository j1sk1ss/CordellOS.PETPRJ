#include <stdint.h>;

#include "stdio.h";
#include "x86.h";
#include "disk.h";

void* _data = (void*)0x20000;

void __attribute__((cdecl)) _start(uint16_t bootDrive) {
    clrscr();

    DISK disk;
    if (!DISK_Initialize(&disk, bootDrive)) {
        printf("Disk init error\r\n");
    }

    DISK_ReadSectors(&disk, 0, 1, _data);
    print_buffer("Buffer: ", _data, 512);
}