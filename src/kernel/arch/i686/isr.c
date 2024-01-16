#include "../../include/isr.h"
#include "../../include/idt.h"
#include "../../include/gdt.h"
#include "../../include/io.h"
#include "../../include/stdio.h"
#include "../../include/vga_text.h"
#include "../../include/keyboard.h"
#include "../../include/date_time.h"
#include "../../include/fat.h"

#include<stddef.h>

ISRHandler _isrHandlers[256];

static const char* const _exceptions[] = {
    "Divide by zero Cordell-error",           "Cordell-Debug",
    "Non-maskable Cordell-Interrupt",         "Cordell-Breakpoint",
    "Cordell-Overflow",                       "Cordell-Bound Range Exceeded",
    "Invalid Cordell-Opcode",                 "Cordell-Device Not Available",
    "Double Cordell-Fault",                   "Coprocessor Cordell-Segment Overrun",
    "Invalid Cordell-TSS",                    "Cordell-Segment Not Present",
    "Cordell-Stack-Segment Fault",            "General Cordell-Protection Fault",
    "Cordell-Page Fault", "",                 "x87 Floating-Point Cordell-Exception",
    "Cordell-Alignment Check",                "Cordell-Machine Check",
    "SIMD Floating-Point Cordell-Exception",  "Virtualization Cordell-Exception",
    "Control Protection Cordell-Exception ",  "", "", "", "", "", "",
    "Hypervisor Injection Cordell-Exception", "VMM Communication Cordell-Exception",
    "Security Cordell-Exception", ""
};

void i686_ISR_InitializeGates();

void syscall(Registers* regs) {
    switch (regs->eax) {
        case SYS_PRINT:
            const char* data = (const char*)regs->ecx;
            kprintf(data);
        break;

        case SYS_PUTC:
            char cdata = (char)regs->ecx;
            kputc(cdata);
        break;

        case SYS_COLOR_PUTC:
            char ccdata = (char)regs->ecx;
            uint8_t color = (uint8_t)regs->ebx;
            kcputc(ccdata, color);
        break;

        case SYS_CLEAR:
            VGA_clrscr();
        break;

        case SYS_SCREEN_COLOR:
            char screen_color = (char)regs->ecx;
            VGA_set_color(screen_color);
        break;

        case SYS_SLEEP:
            const int sleep = (const int)regs->edx;
            for (int i = 0; i < sleep * 100000; i++);
        break;

        case SYS_READ_KEYBOARD:
            char* wait_buffer = (char*)regs->ecx;
            wait_buffer[0] = keyboard_navigation();
        break;

        case SYS_GET_KEY_KEYBOARD:
            char* key_buffer = (char*)regs->ecx;
            char key = '\0';
            if (i686_inb(0x64) & 0x1) {
                key = i686_inb(0x60);
                key = get_character(key);
            }

            key_buffer[0] = key;
        break;

        case SYS_TIME:
            datetime_read_rtc();

            short* date_buffer = (short*)regs->ecx;
            date_buffer[0] = datetime_second;
            date_buffer[1] = datetime_minute;
            date_buffer[2] = datetime_hour;
            date_buffer[3] = datetime_day;
            date_buffer[4] = datetime_month;
            date_buffer[5] = datetime_year;
        break;

        case SYS_MALLOC:
            uint32_t size = regs->ebx;
            void* allocated_memory = malloc(size);
            regs->eax = (uint32_t)allocated_memory;
        break;

        case SYS_FREE:
            void* ptr_to_free = (void*)regs->ebx;
            if (ptr_to_free != NULL)
                free(ptr_to_free);
        break;

        case SYS_READ_FILE:
            char* rfile_path = (char*)regs->ebx;
            struct FATContent* content = FAT_get_content(rfile_path);
            regs->eax = (uint32_t)FAT_read_content(content);
            FAT_unload_content_system(content);
        break;

        case SYS_WRITE_FILE:
            char* wfile_path = (char*)regs->ebx;
            char* fdata = (char*)regs->ecx;
            FAT_edit_content(wfile_path, fdata);
        break;

        case SYS_OPENDIR:
            char *path = (char *)regs->ebx;
            struct FATDirectory* user_dir = (struct FATDirectory*)regs->ecx;
            struct FATDirectory* kdir = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(FAT_get_content(path)->directory->directory_meta), NULL, FALSE);
            memcpy(user_dir, kdir, sizeof(struct FATDirectory));
            FAT_unload_directories_system(kdir);
        break;

        case SYS_EXECUTE_FILE:
        break;

        case SYS_CEXISTS:
        break;

        case SYS_FCREATE:
        break;

        case SYS_FDELETE:
        break;

        case SYS_DIRCREATE:
        break;

        case SYS_DIREDELETE:
        break;
    }
}

void i686_isr_initialize() {
    i686_ISR_InitializeGates();
    for (int i = 0; i < 256; i++)
        i686_idt_enableGate(i);

    i686_idt_enableGate(0x80);
    i686_isr_registerHandler(0x80, syscall);
}

void __attribute__((cdelc)) i686_isr_handler(Registers* regs) {
    if (_isrHandlers[regs->interrupt] != NULL)
        _isrHandlers[regs->interrupt](regs);

    else if (regs->interrupt >= 32) kprintf("Unhandled interrupt! Interrupt: %d\n", regs->interrupt);
    else {
        kprintf("Unhandled exception %d %s\n", regs->interrupt, _exceptions[regs->interrupt]);
        kprintf("  eax=%x  ebx=%x  ecx=%x  edx=%x  esi=%x  edi=%x\n",
               regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
        kprintf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
               regs->esp, regs->ebp, regs->eip, regs->eflag, regs->cs, regs->ds, regs->ss);
        kprintf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);
        kprintf("KERNEL PANIC!\n");
        i686_panic();
    }
}

void i686_isr_registerHandler(int interrupt, ISRHandler handler) {
    _isrHandlers[interrupt] = handler;
    i686_idt_enableGate(interrupt);
}