#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"
#include "include/fat.h"
#include "include/file_system.h"

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
    // FAT and FS initialization
    // - Boot sector 
    // - Cluster data
    // - FS Clusters 

        FAT_initialize();
        // char* fileContents;
        // directory_entry_t fileMeta;
        // FAT_get_file("C:\\", &fileContents, &fileMeta, 1);
        //FAT_directory_list(2, NULL, FALSE);
        //FS_init();

    //////////////////////

    //////////////////////////////////
    // User land part
    // - Shell
    // - File system (current version)

        //user_land_entry();

    //////////////////////////////////
    
end:
    for (;;);
}