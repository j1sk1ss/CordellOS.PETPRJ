ENTRY(boot)
OUTPUT_FORMAT("binary")
phys = 0x700;                  /* BIOS (or MBR) puts us at 0x7C00 */

SECTIONS
{
    . = phys;

    .fsjump                   : { *(.fsjump)          }
    .fsheaders                : { *(.fsheaders)       }

    .entry                    : { __entry_start = .;
                                  *(.entry)   
                                }

    .text                     : { *(.text)            }
    .data                     : { *(.data)            }
    .rodata                   : { *(.rodata)          }
    .bios_footer phys + 0x1FE : { SHORT(0xAA55)       }
    .bss                      : { *(.bss)             }
}