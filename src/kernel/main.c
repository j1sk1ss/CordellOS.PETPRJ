#include <stdint.h>
#include <bitmap.h>

#include "include/hal.h"
#include "include/fat.h"
#include "include/elf.h"
#include "include/tasking.h"
#include "include/x86.h"
#include "include/pit.h"
#include "include/phys_manager.h"
#include "include/virt_manager.h"
#include "include/mouse.h"
#include "include/keyboard.h"
#include "include/allocator.h"
#include "include/syscalls.h"
#include "include/speaker.h"
#include "include/rtl8139.h"
#include "include/pci.h"
#include "include/arp.h"
#include "include/dhcp.h"
#include "include/udp.h"

#include "multiboot.h"


#define CONFIG_PATH     "boot\\boot.txt"
#define CONFIG_KSHELL   0
#define CONFIG_MOUSE    1
#define CONFIG_NETWORK  2
#define CONFIG_ENABLED  '1'
#define CONFIG_DISABLED '0'


extern uint32_t kernel_base;
extern uint32_t kernel_end;


//======================================================================================================================================
// TODO List:                                                     | | Features:                                                      | |
// ? - WIP                                                        | |                                                                | |
// V - complete                                                   | |                                                                | |
//======================================================================================================================================
//      1) Multiboot struct                                       [V]   1) Physical memory manager                                   | |
//      2) Phys and Virt manages                                  [V]   2) Virtual memory manager (paging)                           | |
//          2.0) Make page and phys prints for debug              [?]   3) Multitasking                                              | |
//      3) ELF check v_addr                                       [V]   4) Syscalls                                                  | |
//          3.1) Fix global and static vars                       [V]       4.0) Std libs for userl programs                         | |
//          3.2) Loading ELF without malloc for fdata             [V]       4.1) Cordell-ASM compiler                                | |
//      4) Paging (create error with current malloc) / allocators [V]   5) Mouse & keyboard support                                  | |
//          4.0) Random Page fault (Null error de Italia)         [V]   6) VBE support                                               | |
//          4.1) Tasking with paging                              [V]   7) ELF support                                               | |
//          4.2) ELF exec with tasking and paging                 [V]   8) BMP support                                               | |
//      5) VBE / VESA                                             [V]   9) FAT32/16/12 support                                       | |
//          5.0) VBE kernel                                       [V]   10) Boot config                                              | |
//              5.0.1) Kshell scrolling                           [ ]   11) Multidisk support                                        | |
//          5.1) Double buffering                                 [ ]   12) Networking                                               | |
//      6) Keyboard to int                                        [V]       12.0) DHCP                                               | |
//      7) Reboot outportb(0x64, 0xFE);                           [V]       12.1) UDP                                                | |
//      8) Mouse support                                          [V]       12.2) ARP                                                | |
//          8.0) Std lib for graphics                             [V]       12.3) RLT8139 driver                                     | |
//              8.0.0) Objects                                    [V] 
//              8.0.1) Click event                                [V]
//          8.1) Loading BMP without malloc for fdata             [V]
//          8.1) Syscalls to std libs                             [V]
//              8.1.0) Syscalls for content change                [V]
//              8.1.1) Syscalls for content delete                [V]
//              8.1.2) Syscalls for kmallocp and freep            [V]
//          8.2) VBE userland                                     [ ]
//              8.2.0) VBE file manager                           [?]
//              8.2.1) VBE text editor                            [?]
//      9) Malloc optimization                                    [ ]
//      10) Bugs                                                  [?]
//          10.0) Tasking page fault (In case when we use more    [?]
//                then one task)                                  | |
//          10.1) Mouse page fault                                [?]
//          10.2) Tasking with page allocator                     [V]
//      11) Ethernet                                              [?]
//          11.0) ethernet                                        [?]
//              11.0.0) Send packets                              [V]
//              11.0.1) Receive packets                           [V]
//              11.0.2) DHCP                                      [V]
//              11.0.3) Sending data                              [V]
//                  11.0.3.0) Sending normal data (ndummy)        [V]
//          11.1) IP Udp, Arp                                     [V]
//          11.2) Receive data on Host server                     [V]
//          11.3) STD libs for networking                         [V]
//          11.4) TCP                                             [?]
//          11.5) Host sending to VK/TG etc                       [ ]
//      12) KShell to ELF program                                 [V]
//          12.0) KShell save commands to cash                    [V]
//          12.1) MEM-DATA progs                                  [ ]
//      13) DOOM?                                                 [ ]
//======================================================================================================================================


void kernel_main(struct multiboot_info* mb_info, uint32_t mb_magic, uintptr_t esp) {
    
    //===================
    // GFX init
    // MB Information
    // - Video mode data
    // - MM data
    // - RAM data
    // - VBE data
    //===================

        if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
            kprintf("[%s %i] Multiboot error (Magic is wrong [%u]).\n", __FILE__, __LINE__, mb_magic);
            goto end;
        }

        if (mb_info->vbe_mode != TEXT_MODE) GFX_init(mb_info);
        else _screenBuffer = mb_info->framebuffer_addr;

        kprintf("\n\t = CORDELL KERNEL = \n\t =  [ ver.  10 ]  = \n\n");
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
            kprintf("[%s %i] Virtual memory can`t be init.\n",__FILE__ , __LINE__);
            goto end;
        }

        //===================
        // Map framebuffer to his original phys address
        //===================

            uint32_t framebuffer_pages = gfx_mode.buffer_size / PAGE_SIZE;
            if (framebuffer_pages % PAGE_SIZE > 0) framebuffer_pages++;
    
            // multiplication 2 for hardware
            framebuffer_pages *= 2;
            for (uint32_t i = 0, fb_start = gfx_mode.physical_base_pointer; i < framebuffer_pages; i++, fb_start += PAGE_SIZE)
                map_page((void*)fb_start, (void*)fb_start);

            deinitialize_memory_region(gfx_mode.physical_base_pointer, framebuffer_pages * BLOCK_SIZE);

        //===================
        // Map framebuffer to his original phys address
        //===================

    //===================
    // Tasking init
    // Syscalls init
    // RTL init
    // ARP init
    // HAL initialization
    // - IRQ initialization
    // - GDT initialization
    // - IDT initialization
    // - ISR initialization
    //===================

        HAL_initialize();
        i386_pci_init();
        i386_pit_init();
        i386_syscalls_init();
        i386_task_init();
        
    //===================

    kprintf("Kernel base: %u\nKernel end: %u\n\n", &kernel_base, &kernel_end);

    //===================
    // Drivers
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

        ATA_initialize();
        FAT_initialize();

    //===================
    
    kprintf("FAT driver initialized\nTC:[%u]\tSPC:[%u]\tBPS:[%u]\n\n", total_clusters, sectors_per_cluster, bytes_per_sector);

    //===================
    // Kernel shell part
    //===================

        if (FAT_content_exists(CONFIG_PATH) == 1) {
            struct FATContent* boot_config = FAT_get_content(CONFIG_PATH);
            char* config = FAT_read_content(boot_config);
            FAT_unload_content_system(boot_config);
            
            //===================
            // Network initialization
            //===================

                if (config[CONFIG_NETWORK] == CONFIG_ENABLED) {
                    i386_init_rtl8139();
                    ARP_init();
                    UDP_init();
                    DHCP_discover();
                }

            //===================
            // Network initialization
            //===================

            if (config[CONFIG_MOUSE] == CONFIG_ENABLED)  START_PROCESS("kmouse", PSMS_show);
            if (config[CONFIG_KSHELL] == CONFIG_ENABLED) START_PROCESS("kshell", FAT_ELF_execute_content("boot\\shell\\shell.elf", NULL, NULL));

            kfree(config);
        } else START_PROCESS("kshell", FAT_ELF_execute_content("boot\\shell\\shell.elf", NULL, NULL));

        TASK_start_tasking();
    
    //===================
    // Kernel shell part
    //===================

end:
    kprintf("\n!!KERNEL END!!\n");
    for (;;);
}
