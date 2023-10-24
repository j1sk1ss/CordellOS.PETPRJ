#include <stdint.h>

#include <boot/bootparams.h>

#include <include/memory.h>
#include <include/debug.h>
#include <include/string.h>
#include <include/stdio.h>
#include <include/hal.h>
#include <include/irq.h>
#include <include/io.h>

#include "shell/include/shell.h"

#include "../libs/core/shared/include/mbr.h"
#include "../libs/core/shared/include/fat.h"

extern void _init();

void start(BootParams* bootParams, Partition* part) {
    _init();                            // global constructors
    mm_init(0x50000);                   // Kernel loads in 0x40000 and kernel size is 0x00010000. Malloc start in 0x50000 + 0x1000
    HAL_initialize();
    x86_init_keyboard();

    FAT_File* fd = FAT_open(&part, "/");
    FAT_DirectoryEntry entry;
    int i = 0;

    while (FAT_readEntry(&part, fd, &entry) && i++ < 5) {
        printf("  ");

        for (int i = 0; i < 11; i++)
            putc(entry.Name[i]);

        printf("\r\n");
    }

    FAT_close(fd);

    shell();

    log_debug("Main.c", "Boot device: %x", bootParams->BootDevice);
    
end:
    for (;;);
}