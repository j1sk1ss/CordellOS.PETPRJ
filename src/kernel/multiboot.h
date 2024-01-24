#define GRUB_HEADER_POS  0x00100000

// Define multiboot header constants
#define GRUB_MBALIGN     (1 << 0)
#define GRUB_MEMINFO     (1 << 1)
#define GRUB_MBFLAGS     (MBALIGN | MEMINFO)
#define GRUB_MAGIC       0x1BADB002
#define GRUB_CHECKSUM    -(MAGIC + MBFLAGS)

// Define multiboot header structure
struct multiboot_header {
    unsigned int magic;
    unsigned int flags;
    unsigned int checksum;
};

// Define multiboot memory map entry structure
struct multiboot_memory_map_entry {
    unsigned int size;
    unsigned long long base_addr;
    unsigned long long length;
    unsigned int type;
};