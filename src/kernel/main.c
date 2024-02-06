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

#include "multiboot.h"


extern uint32_t kernel_base;
extern uint32_t kernel_end;


//===================================================================
// TODO List:
//===================================================================
//      1) Multiboot struct                                       [V]
//      2) Phys and Virt manages                                  [?]
//      3) ELF check v_addr                                       [ ]
//          3.1) Fix global and static vars                       [ ]
//      4) Paging (create error with current malloc) / allocators [?]
//          4.1) Tasking with paging                              [ ]
//          4.2) ELF exec with tasking and paging                 [ ]
//      5) VBE / VESA                                             [?]
//      6) Keyboard to int                                        [ ]
//      7) Reboot outportb(0x64, 0xFE);                           [V]
//      8.-1) Syscalls to std libs                                [V]
//      8) DOOM?                                                  [ ]
//===================================================================


void kernel_main(struct multiboot_info* mb_info, uint32_t mb_magic, uintptr_t esp) {
    
    //===================
    // MB Information
    // - Video mode data
    // - MM data
    // - RAM data
    // - VBE data
    //===================

        kprintf("\n = CORDELL KERNEL = \n =   [ ver. 1 ]   = \n\n");

        if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
            kprintf("[kernel.c 54] Multiboot error (Magic is wrong [%u]).\n", mb_magic);
            goto end;
        }

        kprintf("MB FLAGS:        [0x%u]\n", mb_info->flags);
        kprintf("MEM LOW:         [%uKB] => MEM UP: [%uKB]\n", mb_info->mem_lower, mb_info->mem_upper);
        kprintf("BOOT DEVICE:     [0x%u]\n", mb_info->boot_device);
        kprintf("CMD LINE:        [%s]\n", mb_info->cmdline);
        kprintf("VBE MODE:        [%u]\n", mb_info->vbe_mode);

        if (mb_info->vbe_mode == TEXT_MODE) {
            kprintf("\n = VBE INFO = \n\n");
            kprintf("VBE FRAMEBUFFER: [%u]\n", mb_info->framebuffer_addr);
            kprintf("VBE X:           [%u]\n", mb_info->framebuffer_height);
            kprintf("VBE Y:           [%u]\n", mb_info->framebuffer_width);
        }

        //===================
        // Memory test
        // - Fill memory region with dummy data
        // - Read dummy data (should be saved and not changed)
        //===================

            if (mb_info->flags & (1 << 1)) {
                kprintf("\nMEMORY MAP AVALIABLE:\n");
                multiboot_memory_map_t* mmap_entry = (multiboot_memory_map_t*)mb_info->mmap_addr;
                while ((uint32_t)mmap_entry < mb_info->mmap_addr + mb_info->mmap_length) {
                    kprintf("REGION |  LEN: [%u]  |  ADDR: [%u]  |  TYPE: [%u] \n", mmap_entry->len, mmap_entry->addr, mmap_entry->type);
                    if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
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
                                kprintf("Memory test failed at address %p\n", ptr);
                                return;
                            }

                            ++ptr;
                        }

                        kprintf("Memory test passed!\n");
                    }

                    mmap_entry = (multiboot_memory_map_t*)((uint32_t)mmap_entry + mmap_entry->size + sizeof(mmap_entry->size));
                }

                kprintf("\n\n");
            }

        //===================
        // Memory test
        //===================

    //===================
    // Phys & Virt memory manager initialization
    // - Phys blocks
    // - Virt pages
    //===================
    
        uint32_t reserved = &kernel_end;
        uint32_t total_memory = mb_info->mem_upper + (mb_info->mem_lower << 10);
        uint32_t avaliable_memory = total_memory - (reserved + MEM_OFFSET);

        initialize_memory_manager(0x30000, total_memory);
        deinitialize_memory_region(0x00000, reserved);
        initialize_memory_region(reserved + MEM_OFFSET, avaliable_memory);

        // Reboot cause. Error in asm part with cr0 register. Maybe wrong stack creation
        if (initialize_virtual_memory_manager(0x100000) == false) {
            kprintf("[kernel.c 131] Virtual memory can`t be init.\n");
            goto end;
        }
        
    //===================
    // Heap allocator initialization
    // - Initializing first memory block
    //===================

        mm_init(&kernel_end);
        
    //===================
    // HAL initialization
    // - Tasking init
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization
    //===================

        HAL_initialize();
        TASK_task_init();

    //===================

    kprintf("Kernel base: %u\nKernel end: %u\n\n", &kernel_base, &kernel_end);

    //===================
    // Keyboard initialization
    // - Keyboard activation
    //===================

        x86_init_keyboard();

    //===================

    kprintf("Keyboard initialized\n\n");

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
           if (config[0] == '1') kshell();

           FAT_unload_content_system(boot_config);
           kfree(config);
           
        } else kshell();

    //===================
    // User land part
    // - Shell
    // - Idle task
    //===================

        mm_reset(); // TODO: This is odd. Not normal when we should reset manager
        START_PROCESS("userl", FAT_ELF_execute_content("boot\\userl\\userl-s.elf", NULL, NULL));
        TASK_start_tasking();

    //===================
    
end:
    kprintf("\n!!KERNEL END!!\n");
    for (;;);
}
