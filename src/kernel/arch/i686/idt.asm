[bits 32]

global i686_idt_load
i686_idt_load:
    push ebp                    ; save old frame
    mov ebp, esp                ; init new call frame

    ; load gdt
    mov eax, [ebp + 8]
    lidt [eax]                 

    mov esp, ebp                ; restore data
    pop ebp
    ret