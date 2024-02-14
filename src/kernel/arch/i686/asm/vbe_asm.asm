global i386_Video_GetVbeInfo
i386_Video_GetVbeInfo:

    mov ax, 0x4F00
    lea di, [esi + 16]  ; Set DI to the offset where the mode info structure starts
    int 0x10

    cmp ah, 0
    jne error

    ; ES:DI now contains the VBE information block
    ret


global i386_Video_GetModeInfo
i386_Video_GetModeInfo:

    mov ax, 0x4F01
    xor cx, cx  ; Set CX to 0 to request information about the current video mode
    int 0x10

    cmp ah, 0
    jne error

    ; ESI now contains the VBE mode
    mov eax, esi
    ret


global i386_Video_SetMode
i386_Video_SetMode:

    mov ax, 0x4F02
    mov bx, [esp + 4]  ; Get the desired VBE mode from the function argument
    int 0x10
    movzx eax, ah      ; Return AH as the error code
    ret


error:
    mov eax, 0xFFFFFFFF
    ret
 