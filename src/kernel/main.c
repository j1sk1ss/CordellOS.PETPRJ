#include <stdint.h>

#include "include/hal.h"
#include "include/fat.h"
#include "include/elf.h"
#include "include/kshell.h"
#include "include/allocator.h"
#include "include/tasking.h"
#include "include/x86.h"
#include "include/pit.h"
#include "include/phys_manager.h"
#include "include/virt_manager.h"

#include "multiboot.h"


#define KERNEL_POS 0x00100000


extern void _init();
extern uint32_t kernel_base;
extern uint32_t kernel_end;

//===================================================================
// TODO List:
//===================================================================
//      1) Multiboot struct                                       [V]
//      2) Phys and Virt manages                                  [ ]
//      3) ELF check v_addr                                       [ ]
//      4) Paging (create error with current malloc) / allocators [ ]
//          4.1) Tasking with paging                              [ ]
//          4.2) ELF exec with tasking and paging                 [ ]
//      5) VBE / VESA                                             [ ]
//      6) Keyboard to int                                        [ ]
//      7) Reboot outportb(0x64, 0xFE);                           [ ]
//      8.-1) Syscalls to std libs                                [V]
//      8) DOOM?                                                  [ ]
//===================================================================

void kernel_main(void) {

    struct multiboot_header* mb_header = (struct multiboot_header*)GRUB_HEADER_POS;
    struct multiboot_memory_map_entry* memory_map = (struct multiboot_memory_map_entry*)mb_header->flags;

    if (mb_header->magic != GRUB_MAGIC) {
        kprintf("[kernel.c 46] Multiboot error (Magic is wrong).\n");
        goto end;
    }

    //===================
    // Phys & Virt memory manager initialization
    // - Phys blocks
    // - Virt pages
    //===================

        uint32_t total_memory = memory_map->base_addr + memory_map->length - 1;
        initialize_memory_manager(&kernel_end, total_memory);
        initialize_virtual_memory_manager(KERNEL_POS);

    //===================


    //===================
    // GLOBAL CONSTRUCTORS
    // CPP now avaliable
    //===================

        _init();

    //===================


    //===================
    // Heap allocator initialization
    // - Initializing first memory block
    //===================

        mm_init(&kernel_end);

    //===================


    //===================
    // HAL initialization
    // - Tasking init
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization
    //===================

        TASK_task_init();
        HAL_initialize();
        
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


    //===================
    // User land part
    // - Shell
    // - Idle task
    //===================

        START_PROCESS("idle", FAT_ELF_execute_content("boot\\userl\\userl-s.elf", NULL, NULL));
        TASK_start_tasking();

    //===================
    
end:
    for (;;);
}
