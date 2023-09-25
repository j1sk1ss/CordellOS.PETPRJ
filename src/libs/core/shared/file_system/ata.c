#include "ata.h"

#include "../allocator/malloc.h"
#include "../x86/x86.h"

void wait_drive_until_ready() {
    int status = 0;

    do {
        status = x86_inb(BASE_PORT + 7);
    } while ((status ^ 0x80) == 128);
}

void* read_disk_chs(int sector) {
    x86_outb(BASE_PORT + 6, 0x0a0);
    x86_outb(BASE_PORT + 2, 1);
    x86_outb(BASE_PORT + 3, sector);
    x86_outb(BASE_PORT + 4, 0);
    x86_outb(BASE_PORT + 5, 0);
    x86_outb(BASE_PORT + 7, 0x20);

    wait_drive_until_ready();

    short* buffer = malloc(SECTOR_SIZE);

    for (int currByte = 0; currByte < (SECTOR_SIZE / 2); currByte++) 
        buffer[currByte] = x86_inb(BASE_PORT);
    
    return buffer;
}

void* read_disk(int address) {
    x86_outb(BASE_PORT + 6, (0x0e0 | ((address & 0x0F000000) >> 24)));
    x86_outb(BASE_PORT + 2, 1);
    x86_outb(BASE_PORT + 3, address & 0x000000FF);
    x86_outb(BASE_PORT + 4, (address & 0x0000FF00) >> 8);
    x86_outb(BASE_PORT + 5, (address & 0x00FF0000) >> 16);
    x86_outb(BASE_PORT + 7, 0x20);

    wait_drive_until_ready();

    short* buffer = malloc(SECTOR_SIZE);

    for (int currByte = 0; currByte < (SECTOR_SIZE / 2); currByte++) 
        buffer[currByte] = x86_inb(BASE_PORT);

    return buffer;
}

void write_disk_chs(int sector, short* buffer) {
    x86_outb(BASE_PORT + 6, 0x0a0);
    x86_outb(BASE_PORT + 2, 1);
    x86_outb(BASE_PORT + 3, sector);
    x86_outb(BASE_PORT + 4, 0);
    x86_outb(BASE_PORT + 5, 0);
    x86_outb(BASE_PORT + 7, 0x30);

    wait_drive_until_ready();

    for (int currByte = 0; currByte < (SECTOR_SIZE / 2); currByte++) 
        x86_outw(BASE_PORT, buffer[currByte]);

    wait_drive_until_ready();
}

void write_disk(int address, short* buffer) {
    x86_outb(BASE_PORT + 6, (0x0e0 | ((address & 0x0F000000) >> 24)));
    x86_outb(BASE_PORT + 2, 1);
    x86_outb(BASE_PORT + 3, address & 0x000000FF);
    x86_outb(BASE_PORT + 4, (address & 0x0000FF00) >> 8);
    x86_outb(BASE_PORT + 5, (address & 0x00FF0000) >> 16);
    x86_outb(BASE_PORT + 7, 0x30);

    wait_drive_until_ready();

    for (int currByte = 0; currByte < (SECTOR_SIZE / 2); currByte++) 
        x86_outw(BASE_PORT, buffer[currByte]);

    wait_drive_until_ready();
}