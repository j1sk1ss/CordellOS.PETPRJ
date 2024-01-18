#include <stdint.h>
#include <include/hal.h>

#include "include/fat.h"
#include "include/vbe.h"
#include "include/gfx.h"
#include "include/elf.h"
#include "include/keyboard.h"
#include "include/kshell.h"
#include "include/speaker.h"
#include "include/paging.h"
#include "include/allocator.h"


extern void _init();

// TODO List:
// 1) Paging / New allocators 
// 2) Multitasking
// 3) VBE / VESA
// 5) DOOM

void kernel_main(void) {

    ///////////////////////
    // GLOBAL CONSTRUCTORS

        _init();

    ///////////////////////

    ///////////////////////
    // Heap allocator initialization
    // - Initializing first memory block

        mm_init(0x00900000);

    ///////////////////////
    
    ///////////////////////
    //  Paging initialization
    //  - Inits first page

        // Page exception
        // paging_init(0x00900000);

    ///////////////////////

    ///////////////////////
    // HAL initialization
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization

        HAL_initialize();

    ///////////////////////

    ///////////////////////
    // Keyboard initialization
    // - Keyboard activation

        x86_init_keyboard();

    ///////////////////////

    ///////////////////////
    // FAT and FS initialization
    // - Boot sector 
    // - Cluster data
    // - FS Clusters 

        FAT_initialize();

    ///////////////////////

    ///////////////////////
    // Kernel shell part

        kshell();

    ///////////////////////

    ///////////////////////
    // User land part
    // - Shell
    // - File system (current version)

        mm_reset(); // Without some issues happen. TODO: fix free and allocation
        FAT_ELF_execute_content("boot\\userl\\userl.elf", NULL, NULL);
        free(ELF_exe_buffer);

    ///////////////////////
    
end:
    for (;;);
}
