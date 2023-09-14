;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;

; Output byte to port
;
; void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
;

global i686_outb        ; Copies the value from the second operand (source operand) to the I/O port 
                        ; specified with the destination operand (first operand). The source operand 
                        ; can be register AL, AX, or EAX, depending on the size of the port being accessed
                        ; (8, 16, or 32 bits, respectively); the destination operand can be a byte-immediate
                        ; or the DX register. Using a byte immediate allows I/O port addresses 0 to 255 to
                        ; be accessed; using the DX register as a source operand allows I/O ports from 0
                        ; to 65,535 to be accessed.
i686_outb:
    [bits 32]
    
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al

    ret

global i686_inb         ; Copies the value from the I/O port specified with the second operand (source operand)
                        ; to the destination operand (first operand). The source operand can be a byte-immediate
                        ; or the DX register; the destination operand can be register AL, AX, or EAX, depending
                        ; on the size of the port being accessed (8, 16, or 32 bits, respectively).
                        ; Using the DX register as a source operand allows I/O port addresses from 0 to 65,535
                        ; to be accessed; using a byte immediate allows I/O port addresses 0 to 255 to be accessed.
i686_inb:
    [bits 32]
    
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx

    ret

global i686_panic
i686_panic:
    cli
    hlt

global i686_enableInterrupts
i686_enableInterrupts:
    sti
    ret

global i686_disableInterrupts
i686_disableInterrupts:
    cli
    ret


; global x86_inputWait
; x86_inputWait:
;     push eax          ; Save EAX register value
;     push ebx          ; Save EBX register value

;     mov ah, 0         ; Set AH to 0 (AH will be used for the function result)
;     int 16h           ; Call keyboard interrupt

;     movzx eax, ah     ; Zero-extend the result to 32 bits in EAX (assuming AL contains the char)
    
;     pop ebx           ; Restore EBX register value
;     pop eax           ; Restore EAX register value

;     ret