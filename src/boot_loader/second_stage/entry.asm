bits 16

section .entry

; extern _cstart_
global entry

;
;   Entry point
;

entry:
    ; Setup stack

    mov ax, ds
    mov ss, ax
    mov sp, 0
    mov bp, sp
    sti

    ; switch to protected mode
    ; In protected mode operation, the x86 can address 4 GB of address space

    cli                             ; Disable interrupts

    call EnableA20                  ; Enable A20 gate for using more then 1 mb of memory
                                    ; The Intel 8086, Intel 8088, and Intel 80186 processors had 20 address lines,
                                    ; numbered A0 to A19; with these, the processor can access 220 bytes, or 1 MB.
                                    ; Internal address registers of such processors only had 16 bits. To access a 
                                    ; 20-bit address space, an external memory reference was made up of a 16-bit offset 
                                    ; address added to a 16-bit segment number, shifted 4 bits so as to produce a 20-bit
                                    ; physical address.

    call LoadGDT                    ; Load GDT 
                                    ; The Global Descriptor Table (GDT) is a data structure used by Intel x86-family
                                    ; processors starting with the 80286 in order to define the characteristics of
                                    ; the various memory areas used during program execution, including the base address,
                                    ; the size, and access privileges like executability and writability. These memory
                                    ; areas are called segments in Intel terminology.

    mov eax, cr0                    ; set protection enable mode in cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode            ; jmp to protected mode




    ; Boot drive in dl expected
    xor dh, dh
    push dx
    ; call _cstart_

    cli
    hlt
  