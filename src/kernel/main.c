#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"
#include "include/fat.h"
#include "include/iso9660.h"
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
        
        char* fileContents;
        directory_entry_t fileMeta;
        FAT_get_file("boot", &fileContents, &fileMeta, 1);
        FAT_directory_list(GET_CLUSTER_FROM_ENTRY(fileMeta), NULL, FALSE);
        
        directory_entry_t new_file;
        printf("\nADD FILE\n");
        FAT_create_entry(&new_file, "TST", "TXT", FALSE, FAT_allocate_free(), 10);
        if (FAT_name_check(new_file.file_name) != 0)
		    FAT_name2fatname(new_file.file_name);

        printf("FILE ENTRY CREATED!: [%s]\n", new_file.file_name);
        FAT_put_file("boot", "help help", &new_file);

        printf("\n\n");

        char* file1Contents;
        directory_entry_t file1Meta;
        FAT_get_file("boot", &file1Contents, &file1Meta, 1);
        FAT_directory_list(GET_CLUSTER_FROM_ENTRY(fileMeta), NULL, FALSE);

        //FS_init();
        //ISO_init();

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