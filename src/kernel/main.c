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

#include "../libs/include/stdlib.h"


extern uint32_t kernel_base;
extern uint32_t kernel_end;
extern uint32_t grub_header_addr;


//===================================================================
// TODO List:
//===================================================================
//      1) Multiboot struct                                       [V]
//      2) Phys and Virt manages                                  [?]
//      3) ELF check v_addr                                       [ ]
//      4) Paging (create error with current malloc) / allocators [?]
//          4.1) Tasking with paging                              [ ]
//          4.2) ELF exec with tasking and paging                 [ ]
//      5) VBE / VESA                                             [?]
//      6) Keyboard to int                                        [ ]
//      7) Reboot outportb(0x64, 0xFE);                           [V]
//      8.-1) Syscalls to std libs                                [V]
//      8) DOOM?                                                  [ ]
//===================================================================


void kernel_main(void) {
    
    //===================
    // MB Information
    // - Video mode data
    // - MM data
    // - RAM data
    // - VBE data
    //===================

        struct multiboot_header* mb_header = (struct multiboot_header*)(&grub_header_addr);
        multiboot_info_t* mb_info = (multiboot_info_t*)(&grub_header_addr);

        if (mb_header->magic != MULTIBOOT2_HEADER_MAGIC) {
            kprintf("[kernel.c 54] Multiboot error (Magic is wrong [%u]).\n", mb_header->magic);
            goto end;
        }

        if (mb_header->mode_type == TEXT_MODE) {
            kprintf("MB FLAGS:        [0x%u]\n", mb_header->flags);
            kprintf("MEM LOW:         [%uKB] => MEM UP: [%uKB]\n", mb_info->mem_lower, mb_info->mem_upper);
            kprintf("BOOT DEVICE:     [0x%u]\n", mb_info->boot_device);
            kprintf("CMD LINE:        [%s]\n", mb_info->cmdline);

            kprintf("VBE FRAMEBUFFER: [%u]\n", mb_info->framebuffer_addr);
            kprintf("VBE MODE:        [%u]\n", mb_header->mode_type);
            kprintf("VBE X:           [%u]\n", mb_info->framebuffer_height);
            kprintf("VBE Y:           [%u]\n", mb_info->framebuffer_width);
        }

    //===================
    // Phys & Virt memory manager initialization
    // - Phys blocks
    // - Virt pages
    //===================
    
        uint32_t total_memory = mb_info->mem_lower + mb_info->mem_upper;
        initialize_memory_manager(0x30000, total_memory);
        initialize_memory_region(&kernel_end + MEM_OFFSET, total_memory);

        // Reboot cause. Error in asm part with cr0 register. Maybe wrong stack creation
        // if (initialize_virtual_memory_manager(&kernel_end + MEM_OFFSET) == false) {
        //     kprintf("[kernel.c 64] Virtual memory can`t be init.\n");
        //     goto end;
        // }
        
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

        kshell();

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
