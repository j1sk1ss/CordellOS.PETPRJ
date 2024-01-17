;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;

; Output byte to port
;
; void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
;
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
