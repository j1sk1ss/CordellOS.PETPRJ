#include <stdint.h>

#include "include/hal.h"
#include "include/fat.h"
#include "include/elf.h"
#include "include/kshell.h"
#include "include/tasking.h"
#include "include/x86.h"
#include "include/pit.h"
#include "include/phys_manager.h"
#include "include/virt_manager.h"
#include "include/mouse.h"
#include "include/keyboard.h"
#include "include/allocator.h"
#include "include/syscalls.h"

#include "multiboot.h"


extern uint32_t kernel_base;
extern uint32_t kernel_end;


//===================================================================
// TODO List:
//===================================================================
//      1) Multiboot struct                                       [V]
//      2) Phys and Virt manages                                  [V]
//      3) ELF check v_addr                                       [V]
//          3.1) Fix global and static vars                       [V]
//          3.2) Loading ELF without malloc for fdata             [V]
//      4) Paging (create error with current malloc) / allocators [V]
//          4.0) Random Page fault (Null error de Italia)         [V]
//          4.1) Tasking with paging                              [V]
//          4.2) ELF exec with tasking and paging                 [V]
//      5) VBE / VESA                                             [V]
//          5.0) VBE kernel                                       [V]
//          5.1) Double buffering                                 [ ]
//      6) Keyboard to int                                        [V]
//      7) Reboot outportb(0x64, 0xFE);                           [V]
//      8) Mouse support                                          [V]
//          8.0) Std lib for graphics                             [?]
//              8.0.0) Objects                                    [V]
//              8.0.1) Click event                                [ ]
//          8.1) Loading BMP without malloc for fdata             [V]
//          8.1) Syscalls to std libs                             [V]
//          8.2) VBE userland                                     [ ]
//      9) Malloc optimization                                    [?]
//      10) Bags                                                  [?]
//          10.0) Tasking page fault                              [?]
//          10.1) Mouse page fault                                [V]
//          10.2) Tasking with page allocator                     [ ]
//      11) DOOM?                                                 [ ]
//===================================================================


void kernel_main(struct multiboot_info* mb_info, uint32_t mb_magic, uintptr_t esp) {
    
    //===================
    // MB Information
    // - Video mode data
    // - MM data
    // - RAM data
    // - VBE data
    //===================

        kprintf("\n\t = CORDELL KERNEL = \n\t =   [ ver. 7 ]   = \n\n");

        if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
            kprintf("[kernel.c 73] Multiboot error (Magic is wrong [%u]).\n", mb_magic);
            goto end;
        }

        if (mb_info->vbe_mode != TEXT_MODE) GFX_init(mb_info);

        kprintf("\n\t =  GENERAL INFO  = \n");
        kprintf("MB FLAGS:        [0x%u]\n", mb_info->flags);
        kprintf("MEM LOW:         [%uKB] => MEM UP: [%uKB]\n", mb_info->mem_lower, mb_info->mem_upper);
        kprintf("BOOT DEVICE:     [0x%u]\n", mb_info->boot_device);
        kprintf("CMD LINE:        [%s]\n", mb_info->cmdline);
        kprintf("VBE MODE:        [%u]\n", mb_info->vbe_mode);

        kprintf("\n\t =    VBE INFO    = \n");
        kprintf("VBE FRAMEBUFFER: [%u]\n", mb_info->framebuffer_addr);
        kprintf("VBE Y:           [%u]\n", mb_info->framebuffer_height);
        kprintf("VBE X:           [%u]\n", mb_info->framebuffer_width);
        kprintf("VBE BPP:         [%u]\n", mb_info->framebuffer_bpp);

    //===================
    // Phys & Virt memory manager initialization
    // - Phys blocks
    // - Virt pages
    //===================
    
        uint32_t total_memory = mb_info->mem_upper + (mb_info->mem_lower << 10);
        initialize_memory_manager(0x30000, total_memory);

        //===================
        // Memory test
        // - Fill memory region with dummy data
        // - Read dummy data (should be saved and not changed)
        //===================

            if (mb_info->flags & (1 << 1)) {
                kprintf("\n\t =  MEMORY  INFO  = \n");
                multiboot_memory_map_t* mmap_entry = (multiboot_memory_map_t*)mb_info->mmap_addr;
                while ((uint32_t)mmap_entry < mb_info->mmap_addr + mb_info->mmap_length) {
                    if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                        kprintf("REGION |  LEN: [%u]  |  ADDR: [%u]  |  TYPE: [%u] \t", mmap_entry->len, mmap_entry->addr, mmap_entry->type);
                        const uint32_t pattern = 0xC08DE77;

                        uint32_t* ptr = (uint32_t*)mmap_entry->addr;
                        uint32_t* end = (uint32_t*)(mmap_entry->addr + mmap_entry->len);
                        while (ptr < end) {
                            *ptr = pattern;
                            ++ptr;
                        }

                        ptr = (uint32_t*)mmap_entry->addr;
                        while (ptr < end) {
                            if (*ptr != pattern) {
                                kprintf("memory test failed at address %u\n", ptr);
                                return;
                            }

                            ++ptr;
                        }

                        kprintf("memory test passed!\n");
                        initialize_memory_region(mmap_entry->addr, mmap_entry->len);
                    }

                    mmap_entry = (multiboot_memory_map_t*)((uint32_t)mmap_entry + mmap_entry->size + sizeof(mmap_entry->size));
                }

                kprintf("\n\n");
            }

        //===================
        // Memory test
        //===================

        deinitialize_memory_region(0x1000, 0x11000);
        deinitialize_memory_region(0x30000, max_blocks / BLOCKS_PER_BYTE);
        if (initialize_virtual_memory_manager(0x100000) == false) {
            kprintf("[kernel.c 149] Virtual memory can`t be init.\n");
            goto end;
        }

        //===================
        // Map framebuffer to his original phys address
        //===================

            uint32_t framebuffer_pages = (gfx_mode.y_resolution * gfx_mode.x_resolution * (gfx_mode.bits_per_pixel / 2)) / PAGE_SIZE;
            if (framebuffer_pages % PAGE_SIZE > 0) framebuffer_pages++;
    
            framebuffer_pages *= 2;
            for (uint32_t i = 0, fb_start = gfx_mode.physical_base_pointer; i < framebuffer_pages; i++, fb_start += PAGE_SIZE)
                map_page((void*)fb_start, (void*)fb_start);

            deinitialize_memory_region(gfx_mode.physical_base_pointer, framebuffer_pages * BLOCK_SIZE);

        //===================
        // Map framebuffer to his original phys address
        //===================

    //===================
    // HAL initialization
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization
    // - Tasking init
    // - Syscalls init
    //===================

        HAL_initialize();
        i386_syscalls_init();
        i386_task_init();

    //===================

    kprintf("Kernel base: %u\nKernel end: %u\n\n", &kernel_base, &kernel_end);

    //===================
    // Keyboard & Mouse initialization
    // - Keyboard activation
    // - Mouse activation
    //===================

        i386_init_keyboard();
        i386_init_mouse();

    //===================

    kprintf("Keyboard & mouse initialized [%i]\n\n", i386_detect_ps2_mouse());

    //===================
    // FAT and FS initialization
    // - Boot sector 
    // - Cluster data
    // - FS Clusters 
    //===================

        FAT_initialize();

    //===================
    
    kprintf("FAT driver initialized\nTC:[%u]\tSPC:[%u]\tBPS:[%u]\n\n", total_clusters, sectors_per_cluster, bytes_per_sector);

    //===================
    // Kernel shell part
    //===================

        if (FAT_content_exists("boot\\boot.txt") == 1) {
            struct FATContent* boot_config = FAT_get_content("boot\\boot.txt");
            char* config = FAT_read_content(boot_config);
            FAT_unload_content_system(boot_config);
            
            if (config[1] == '1') START_PROCESS("kmouse", PSMS_show);
            if (config[0] == '1') START_PROCESS("kshell", kshell);

            kfree(config);
        } else START_PROCESS("kshell", kshell);

        TASK_start_tasking();
    
    //===================
    // Kernel shell part
    //===================

end:
    kprintf("\n!!KERNEL END!!\n");
    for (;;);
}
