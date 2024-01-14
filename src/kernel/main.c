#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"

#include "include/fat.h"
#include "include/gfx.h"
#include "include/elf.h"
#include "include/keyboard.h"

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

    //////////////////////

    //////////////////////////////////
    // User land part
    // - Shell
    // - File system (current version)

        //user_land_entry();
        GFX_init();
        printf("RESOLUTION: [%i]\t[%i]\n", gfx_mode->x_resolution, gfx_mode->y_resolution);
        // Point p0, p1, p2;
        // Point vertex_array[6];

        // // Draw pixel test
        // for (uint8_t i = 0; i < 100; i ++) {
        //     for (uint8_t j = 0; j < 100; j++) {
        //         p0.X = 200 + j;
        //         p0.Y = 200 + i;
        //         GFX_draw_pixel(p0.X, p0.Y, GFX_convert_color(RED));
        //     }
        // } 

    //////////////////////////////////
    
end:
    for (;;);
}
