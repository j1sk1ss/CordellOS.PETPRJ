ENTRY(entry)
OUTPUT_FORMAT("binary")
phys = 0x00000500;

/* On Unix and Unix-like systems, the linker is known as "ld". Origins of the name "ld" are 
   "LoaDer" and "Link eDitor". The term "loader" was used to describe the process of loading
   external symbols from other programs during the process of linking.
*/

SECTIONS {
     . = phys;

     .entry             : { __entry_start = .;      *(.entry)   }
     .text              : {__text_start = .;        *(.text)    }
     .data              : {__data_start = .;        *(.data)    }
     .rodata            : {__rodata_start = .;      *(.rodata)  }
     .bss               : {__bss_start = .;         *(.bss)     }

     __end = .;
}