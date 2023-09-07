;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;

;
; Enter protected mode
;
%macro x86_enter_protected_mode 0
    [bits 16]                                               ; x86-64 processors in Real mode act exactly like 16
                                                            ; bit chips, and x86-64 chips in protected mode act exactly
                                                            ; like 32-bit processors. To unlock the 64-bit capabilities
                                                            ; of the chip, the chip must be switched into Long Mode.
    cli
    
    ; set "protection enable" flag in control register 0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; far jump into protected mode
    jmp dword 08h:.pmode

    .pmode:
        ; we are now in protected mode!
        [bits 32]
        
        ; 6 - setup segment registers
        mov ax, 0x10
        mov ds, ax
        mov ss, ax

%endmacro

%macro x86_enter_real_mode 0
    [bits 32]
    cli

    ; jump to 16-bit protected mode
    jmp word 18h:.pmode16

    .pmode16:
        [bits 16]

        ; disable protected mode
        mov eax, cr0
        and al, ~1
        mov cr0, eax

        ; jump to real mode
        jmp word 00h:.rmode

        .rmode:

        ; 4 - setup segments
        mov ax, 0
        mov ds, ax
        mov ss, ax

        ; 5 - enable interrupts
        sti

%endmacro


;
; Convert linear address to segment:offset address
; Args: 1 - linear address
;       2 - target segment (e.g. es)
;       3 - target 32-bit register to use (e.g. eax)
;       4 - target lower 16-bit half of #3 (e.g. ax)
; Result: segment:offset is in 2:4 (e.g. es:ax)
;

%macro linear_to_seg_offset 4 
                    ; %X is parameter
    mov %3, %1      ; eax = linear address
    shr %3, 4       ; eax >> 4 to get segment
    mov %2, %4      ;  es = segment
    mov %3, %1      ; eax = linear address
    and %3, 0xf

%endmacro


;
; Output byte to port
;
; void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
;

global x86_outb         ; Copies the value from the second operand (source operand) to the I/O port 
                        ; specified with the destination operand (first operand). The source operand 
                        ; can be register AL, AX, or EAX, depending on the size of the port being accessed
                        ; (8, 16, or 32 bits, respectively); the destination operand can be a byte-immediate
                        ; or the DX register. Using a byte immediate allows I/O port addresses 0 to 255 to
                        ; be accessed; using the DX register as a source operand allows I/O ports from 0
                        ; to 65,535 to be accessed.
x86_outb:
    [bits 32]
    
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al

    ret

global x86_inb          ; Copies the value from the I/O port specified with the second operand (source operand)
                        ; to the destination operand (first operand). The source operand can be a byte-immediate
                        ; or the DX register; the destination operand can be register AL, AX, or EAX, depending
                        ; on the size of the port being accessed (8, 16, or 32 bits, respectively).
                        ; Using the DX register as a source operand allows I/O port addresses from 0 to 65,535
                        ; to be accessed; using a byte immediate allows I/O port addresses 0 to 255 to be accessed.
x86_inb:
    [bits 32]
    
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx

    ret


global x86_disk_getDriveParams
x86_disk_getDriveParams:
    [bits 32]

    ; make new call frame
    push ebp                ; save old call frame
    mov ebp, esp            ; initialize new call frame

    x86_enter_real_mode
    
    [bits 16]

    ; save regs
    push es
    push bx
    push esi
    push di

    ; call int13h
    mov dl, [bp + 8]        ; dl - disk drive
    mov ah, 08h
    mov di, 0               ; es:di - 0000:0000
    mov es, di
    stc
    int 13h

    ; out params
    mov eax, 1
    sbb eax, 0

    ; drive type from bl
    linear_to_seg_offset [bp + 12], es, esi, si 
    mov es:[si], bl

    ; cylinders info
    mov bl, ch              ; cylinders - lower bits in ch
    mov bh, cl              ; cylinders - upper bits in cl (6-7)
    shr bh, 6
    inc bx

    linear_to_seg_offset [bp + 16], es, esi, si 
    mov [es:si], bx

    ; number of sectors
    xor ch, ch              ; sectors - lower 5 bits in cl
    and cl, 3Fh

    linear_to_seg_offset [bp + 20], es, esi, si 
    mov [es:si], cx

    ; head info
    mov cl, dh              ; heads - dh
    inc cx
    
    linear_to_seg_offset [bp + 24], es, esi, si 
    mov [es:si], cx

    ; restore regs
    pop di
    pop esi
    pop bx
    pop es

    ; return
    push eax

    x86_enter_protected_mode

    pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret


global x86_diskRead
x86_diskRead:

    ; make new call frame
    push ebp                    ; save old call frame
    mov ebp, esp                ; initialize new call frame

    x86_enter_real_mode

    ; save modified regs
    push ebx
    push es

    ; setup args
    mov dl, [bp + 8]            ; dl - drive

    mov ch, [bp + 12]           ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]           ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]           ; cl - sector to bits 0-5
    and al, 3Fh
    or cl, al

    mov dh, [bp + 20]           ; dh - head

    mov al, [bp + 24]           ; al - count

    linear_to_seg_offset [bp + 28], es, ebx, bx

    ; call int13h
    mov ah, 02h
    stc
    int 13h

    ; set return value
    mov eax, 1
    sbb eax, 0              ; 1 on success, 0 on fail   

    ; restore regs
    pop es
    pop ebx

    push eax                 ; Save eax return

    x86_enter_protected_mode

    pop eax                  ; Restore eax return

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret


global x86_diskReset
x86_diskReset:
    [bits 32]

    ; make new call frame
    push ebp                ; save old call frame
    mov ebp, esp            ; initialize new call frame

    x86_enter_real_mode

    mov ah, 0
    mov dl, [bp + 8]        ; dl - drive
    stc
    int 13h

    mov eax, 1
    sbb eax, 0              ; 1 on success, 0 on fail   

    push eax                ; save eax return

    x86_enter_protected_mode

    pop eax                 ; restore eax retuen

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
