#include <stdint.h>
#include <include/hal.h>

#include "include/fat.h"
#include "include/elf.h"
#include "include/kshell.h"
#include "include/allocator.h"
#include "include/tasking.h"
#include "include/x86.h"
#include "include/pit.h"


extern void _init();
extern uint32_t kernel_base;
extern uint32_t kernel_end;

// TODO List:
// Phys and Virt manages. Paging  https://github.com/stevej/osdev/blob/master/kernel/mem/mem.c#L238 (Look memory.c)
// ELF check v_addr
// 1) Paging (create error with current malloc) / New allocators 
// 2) Multitasking 
// 3) VBE / VESA
// Keyboard to int
// Reboot outportb(0x64, 0xFE);
// 5) DOOM

int gla = 0;

void a() {
    while(1) {
        kprintf("hello\n");
    }
}

void b() {
    while(1) {
        VGA_putchr(20, 20, 'b');
    }
}

void kernel_main(void) {

    //===================
    // GLOBAL CONSTRUCTORS
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
    //  Paging initialization
    //  - Inits first page
    //===================

        // Page exception
        // paging_init();

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

    // Multitasking

    TASK_add_task(TASK_create_task((uint32_t)b));
    TASK_add_task(TASK_create_task((uint32_t)a));
    TASK_start_tasking();

    // Multitasking


    //===================
    // Kernel shell part
    //===================

        //kshell();

    //===================

    //===================
    // User land part
    // - Shell
    // - File system (current version)
    //===================

        //FAT_ELF_execute_content("boot\\userl\\userl-s.elf", NULL, NULL);

    //===================
    
end:
    for (;;);
}
