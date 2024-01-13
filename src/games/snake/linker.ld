ENTRY(main)
OUTPUT_FORMAT("elf32-i386")

SECTIONS {
    . = 0M;

    /* Program code */
    .text BLOCK(4K) : ALIGN(4K) {
        __text_start = .;
        *(.text)
        *(.rodata)
        . = ALIGN(4K);
        __text_end = .;
    }

    /* Data sections */
    .data BLOCK(4K) : ALIGN(4K) {
        __data_start = .;
        *(.data)
        . = ALIGN(4K);
        __data_end = .;
    }

    /* Read-only data sections */
    .rodata BLOCK(4K) : ALIGN(4K) {
        __rodata_start = .;
        *(.rodata)
        . = ALIGN(4K);
        __rodata_end = .;
    }

    /* BSS section (uninitialized data) */
    .bss BLOCK(4K) : ALIGN(4K) {
        __bss_start = .;
        *(.bss)
        . = ALIGN(4K);
        __bss_end = .;
    }

    __end = .;
}