;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;


bits 16

section .entry

extern start        ; c start

extern __bss_start  ; sections
extern __end

global entry

;
;   Entry point
;

entry:
    cli                             ; Disable interrupts

    mov ax, 0                       ; setup segments
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0xFFF0
    mov ebp, esp                    ; Setup stack at 0xFFF0
    sti

    mov [boot_drive], dl            ; save boot drive

    ; switch to protected mode
    ; In protected mode operation, the x86 can address 4 GB of address space

    cli

    call enable_a20                 ; Enable A20 gate for using more then 1 mb of memory
                                    ; The Intel 8086, Intel 8088, and Intel 80186 processors had 20 address lines,
                                    ; numbered A0 to A19; with these, the processor can access 220 bytes, or 1 MB.
                                    ; Internal address registers of such processors only had 16 bits. To access a 
                                    ; 20-bit address space, an external memory reference was made up of a 16-bit offset 
                                    ; address added to a 16-bit segment number, shifted 4 bits so as to produce a 20-bit
                                    ; physical address.

    call load_gdt                   ; Load GDT 
                                    ; The Global Descriptor Table (GDT) is a data structure used by Intel x86-family
                                    ; processors starting with the 80286 in order to define the characteristics of
                                    ; the various memory areas used during program execution, including the base address,
                                    ; the size, and access privileges like executability and writability. These memory
                                    ; areas are called segments in Intel terminology.

    mov eax, cr0                    ; set protection enable mode in cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode            ; jmp to protected mode


    ;
    ;   Protected mode part
    ;

    .pmode:
        [bits 32]
        
        mov ax, 0x10                    ; 6 - setup segment registers
        mov ds, ax
        mov ss, ax

        ; clear bss (uninit)
        mov edi, __bss_start
        mov ecx, __end
        mov ecx, __bss_start
        mov al, 0
        cld
        rep stosb                       ; repeats instruction decrementing ECX until zero
                                        ; and stores value from AL incrementing ES:EDI

        ;
        ; Part where expected boot drive
        ; Cuz we in 32 bits, used edx registers instead dx
        ;

        ; call C
        push dword[boot_drive]
        call start

        cli                             ; Clear interrupt flags
        hlt                             ; halt the CPU until the next external interrupt is fired


    enable_a20:
        [bits 16]
        ; disable keyboard
        call wait_input_a20
        mov al, KbdControllerDisableKeyboard
        out KbdControllerCommandPort, al

        ; read control output port
        call wait_input_a20
        mov al, KbdControllerReadCtrlOutputPort
        out KbdControllerCommandPort, al

        call wait_output_a20
        in al, KbdControllerDataPort
        push eax

        ; write control output port
        call wait_input_a20
        mov al, KbdControllerWriteCtrlOutputPort
        out KbdControllerCommandPort, al
        
        call wait_input_a20
        pop eax
        or al, 2                                    ; bit 2 = A20 bit
        out KbdControllerDataPort, al

        ; enable keyboard
        call wait_input_a20
        mov al, KbdControllerEnableKeyboard
        out KbdControllerCommandPort, al

        call wait_input_a20
        ret


    wait_input_a20:
        [bits 16]
        ; wait until status bit 2 (input buffer) is 0
        ; by reading from command port, we read status byte
        in al, KbdControllerCommandPort
        test al, 2
        jnz wait_input_a20
        ret

    wait_output_a20:
        [bits 16]
        ; wait until status bit 1 (output buffer) is 1 so it can be read
        in al, KbdControllerCommandPort
        test al, 1
        jz wait_output_a20
        ret


    load_gdt:
        [bits 16]
        lgdt [gdt_desc]
        ret



        KbdControllerDataPort               equ 0x60
        KbdControllerCommandPort            equ 0x64
        KbdControllerDisableKeyboard        equ 0xAD
        KbdControllerEnableKeyboard         equ 0xAE
        KbdControllerReadCtrlOutputPort     equ 0xD0
        KbdControllerWriteCtrlOutputPort    equ 0xD1

        ScreenBuffer                        equ 0xB8000

        gdt_function:      ; NULL descriptor
                    dq 0

                    ; 32-bit code segment
                    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
                    dw 0                        ; base (bits 0-15) = 0x0
                    db 0                        ; base (bits 16-23)
                    db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
                    db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
                    db 0                        ; base high

                    ; 32-bit data segment
                    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
                    dw 0                        ; base (bits 0-15) = 0x0
                    db 0                        ; base (bits 16-23)
                    db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
                    db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
                    db 0                        ; base high

                    ; 16-bit code segment
                    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
                    dw 0                        ; base (bits 0-15) = 0x0
                    db 0                        ; base (bits 16-23)
                    db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
                    db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
                    db 0                        ; base high

                    ; 16-bit data segment
                    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
                    dw 0                        ; base (bits 0-15) = 0x0
                    db 0                        ; base (bits 16-23)
                    db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
                    db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
                    db 0                        ; base high

        gdt_desc:   dw gdt_desc - gdt_function - 1      ; limit = size of GDT
                    dd gdt_function                     ; address of GDT

        boot_drive: db 0