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
        FS_init();
        // printf("\nMAIN DIR CREATED. NAME: [%s]", FS_get_main_directory()->name);

        // FS_create_directory("/testDir");
        // printf("\nDIR CREATED. NAME: [%s]", FS_global_find_directory("/testDir")->name);

        // FS_create_file(0, 0, 0, "/testDir/testFile","txt", 0);
        // printf("\nFILE CREATED. EXT: [%s]", FS_global_find_file("/testDir/testFile")->extension);

        // if (FS_global_find_directory("/tesasdtDir") == NULL)
        //     printf("\n[tesasdtDir] NOT FOUNDED");
        
        // if (FS_global_find_directory("/testDir") != NULL)
        //     printf("\n[testDir]  FOUNDED");

        // if (FS_global_find_file("/testDir/asd") == NULL)
        //     printf("\n[asd] NOT FOUNDED");
        
        // if (FS_global_find_file("/testDir/testFile") != NULL)
        //     printf("\n[testFile] FOUNDED");

        // printf("\nDELETING DIR\n");
        // FS_delete_directory("testDir", FS_global_find_directory("/"));
        // if (FS_global_find_directory("/testDir") == NULL)
        //     printf("\n[testDir] NOT FOUNDED");
        // else 
        //     printf("FOUNDED DIR: [%s]", FS_global_find_directory("/testDir")->name);

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