ENTRY(boot)
OUTPUT_FORMAT("binary")

phys = 0x7C00;

/* On Unix and Unix-like systems, the linker is known as "ld". Origins of the name "ld" are 
   "LoaDer" and "Link eDitor". The term "loader" was used to describe the process of loading
   external symbols from other programs during the process of linking.
*/

SECTIONS {

    . = phys;
    .fsjump             : { *(.fsjump)          }
    .fsheaders          : { *(.fsheaders)       }

    .entry              : { __entry_start = .;
                            *(.entry)   
                          }

    .text               : { *(.text)            }
    .data               : { *(.data)            }
    .rodata             : { *(.rodata)          }
    .bios_footer 0x7DFE : { SHORT(0xAA55)       }
    .bss                : { *(.bss)             }

}