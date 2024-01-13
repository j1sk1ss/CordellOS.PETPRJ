/* The bootloader will look at this image and start execution at the symbol designated at the entry point. */
ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")

SECTIONS {
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
       loaded at by the bootloader. */
    . = 1M;

    /* First put the multiboot header, as it is required to be put very early in the image or the bootloader won't recognize the file format. Next we'll put the .text section. */
    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
}