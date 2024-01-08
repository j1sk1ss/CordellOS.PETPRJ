#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"
#include "include/fat.h"
#include "include/keyboard.h"
//#include "include/file_system.h"

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
        
        // TEST DIR LST

        printf("\n");
        struct FATContent* content = FAT_get_content(keyboard_read(VISIBLE_KEYBOARD, FOREGROUND_WHITE + BACKGROUND_BLACK), 1);
        struct FATDirectory* directory = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE);

        VGA_clrscr();
        // struct FATDirectory* current = directory->subDirectory;
        // while (current != NULL) {
        //     printf("[%s]\t\tDIR\n", current->name);
        //     current = current->next;
        // }            

        // struct FATFile* current_file = directory->files;
        // while (current_file != NULL) {
        //     printf("[%s]\t[%s]\tFILE\n", current_file->name, current_file->extension);
        //     current_file = current_file->next;
        // }  

        //


        // TEST CREATION OF FILE

        // printf("\nADD FILE\n");
        // struct directory_entry* new_file = FAT_create_entry("TST", "TXT", FALSE, NULL, 41);

        // printf("FILE ENTRY CREATED!: [%s]\n", new_file->file_name);
        // FAT_put_content("boot", "TESTTESTTESTTESTTESTTESTTESTTESTTESTTEST", new_file);
        // printf("\nADDED FILE\n");

        // printf("\n\n");

        //


        // TEST CREATION OF DIRECTORY

        // printf("\nADD DIR\n");

        // struct FATContent* content = malloc(sizeof(struct FATContent));
        // content->directory = malloc(sizeof(struct FATDirectory));
        // content->file = NULL;

        // content->directory->directory_meta = *FAT_create_entry("CRDL", NULL, TRUE, NULL, NULL);

        // FAT_put_content("boot", content);

        // printf("\n\n");

        //

        // free(content);
        // content = FAT_get_content("boot", 1);
        // struct FATDirectory* directory = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(content->directory->directory_meta), NULL, FALSE);

        // struct FATDirectory* current = directory->subDirectory;
        // while (current != NULL) {
        //     printf("[%s]\t\tDIR\n", current->directory_meta.file_name);
        //     current = current->next;
        // }

        // END TEST LS

        // char* file1Contents;
        // directory_entry_t file1Meta;
        // FAT_get_content("boot", &file1Contents, &file1Meta, 1);
        // FAT_directory_list(GET_CLUSTER_FROM_ENTRY(fileMeta), NULL, FALSE);

        //


        // END TEST CONTENTS

        // FAT_get_content("boot\\tst.txt", &file1Contents, &file1Meta, 1);
        // printf("\n");
        // printf(file1Contents);

        //

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