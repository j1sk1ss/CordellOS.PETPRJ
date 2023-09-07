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

global crash
crash:
    mov eax, 0
    div eax
    ret