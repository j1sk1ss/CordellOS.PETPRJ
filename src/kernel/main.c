#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"
#include "include/fat.h"

extern void _init();

void kernel_main(void) {

    ///////////////////////
    // GLOBAL CONSTRUCTORS

        _init();

    ///////////////////////

    ////////////////////////////////////
    // Heap allocator initialization
    // - Initializing first memory block

        mm_init(0x00200000);

    ////////////////////////////////////
    
    ///////////////////////
    // HAL initialization
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization

        HAL_initialize();

    ///////////////////////

    //////////////////////////
    // Keyboard initialization
    // - Keyboard activation

        x86_init_keyboard();

    //////////////////////////

    /////////////////////
    // FAT initialization
    // - Boot sector 
    // - Cluster data

        FATInitialize();

    //////////////////////

    //////////////////////////////////
    // User land part
    // - Shell
    // - File system (current version)

        user_land_entry();

    //////////////////////////////////
    
end:
    for (;;);
}