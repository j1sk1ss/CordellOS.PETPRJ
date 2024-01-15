#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"

#include "include/fat.h"
#include "include/vbe.h"
#include "include/gfx.h"
#include "include/elf.h"
#include "include/keyboard.h"

#define COLOR(r,g,b) ((b) | (g << 8) | (r << 16))

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

        // GRAPHIC TEST
        //GFX_init();
        // const int desiredWidth = 1024;
        // const int desiredHeight = 768;
        // const int desiredBpp = 32;
        // uint16_t pickedMode = 0xffff;

        // VbeInfoBlock* info = (VbeInfoBlock*)malloc(sizeof(VbeInfoBlock));
        // VbeModeInfo* modeInfo = (VbeModeInfo*)malloc(sizeof(VbeModeInfo));
        // if (VBE_GetControllerInfo(info)) {
        //     uint16_t* mode = (uint16_t*)(info->VideoModePtr);
        //     for (int i = 0; mode[i] != 0xFFFF; i++) {
        //         if (!VBE_GetModeInfo(mode[i], modeInfo)) {
        //             kprintf("Can't get mode info %x :(\n", mode[i]);
        //             continue;
        //         }

        //         bool hasFB = (modeInfo->attributes & 0x90) == 0x90;
        //         if (hasFB && modeInfo->width == desiredWidth && modeInfo->height == desiredHeight && modeInfo->bpp == desiredBpp) {
        //             pickedMode = mode[i];
        //             break;
        //         }
        //     }

        //     if (pickedMode != 0xFFFF && VBE_SetMode(pickedMode)) {
        //         uint32_t* fb = (uint32_t*)(modeInfo->framebuffer);
        //         int w = modeInfo->width;
        //         int h = modeInfo->height;
        //         for (int y = 0; y < h; y++) {
        //             for (int x = 0; x < w; x++) {
        //                 fb[y * modeInfo->pitch / 4 + x] = COLOR(x, y, x+y);
        //             }
        //         }
        //     }
        // }

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
