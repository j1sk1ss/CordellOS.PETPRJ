; Convert linear address to segment:offset address
; Args:
;    1 - linear address
;    2 - (out) target segment (e.g. es)
;    3 - target 32-bit register to use (e.g. eax)
;    4 - target lower 16-bit half of #3 (e.g. ax)

%macro LinearToSegOffset 4

    mov %3, %1      ; linear address to eax
    shr %3, 4
    mov %2, %4
    mov %3, %1      ; linear address to eax
    and %3, 0xf

%endmacro


global x86_Video_GetVbeInfo
x86_Video_GetVbeInfo:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    ; save modified regs
    push edi
    push es
    push ebp                ; bochs vbe changes ebp

    ; call interrupt
    mov ax, 0x4f00
    LinearToSegOffset [bp + 8], es, edi, di
    int 10h

    ; check return
    cmp al, 4fh
    jne .error
    
    ; put status in eax
    mov al, ah
    and eax, 0xFF
    jmp .cont

.error:
    mov eax, -1

.cont:
    ; restore regs
    pop ebp                ; bochs vbe changes ebp
    pop es
    pop ebx

    push eax

    pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret



global x86_Video_GetModeInfo
x86_Video_GetModeInfo:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    ; save modified regs
    push edi
    push es
    push ebp                ; bochs vbe changes ebp
    push ecx

    ; call interrupt
    mov ax, 0x4f01
    mov cx, [bp + 8]
    LinearToSegOffset [bp + 12], es, edi, di
    int 10h

    ; check return
    cmp al, 4fh
    jne .error
    
    ; put status in eax
    mov al, ah
    and eax, 0xFF
    jmp .cont

.error:
    mov eax, -1

.cont:
    ; restore regs
    pop ecx
    pop ebp                ; bochs vbe changes ebp
    pop es
    pop ebx

    push eax

    pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret


global x86_Video_SetMode
x86_Video_SetMode:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    ; save modified regs
    push edi
    push es
    push ebp                ; bochs vbe changes ebp
    push ebx

    ; call interrupt
    mov ax, 0
    mov es, ax
    mov edi, 0
    mov ax, 0x4f02
    mov bx, [bp + 8]
    int 10h

    ; check return
    cmp al, 4fh
    jne .error
    
    ; put status in eax
    mov al, ah
    and eax, 0xFF
    jmp .cont

.error:
    mov eax, -1

.cont:
    ; restore regs
    pop ebx
    pop ebp                ; bochs vbe changes ebp
    pop es
    pop ebx

    push eax

    pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret