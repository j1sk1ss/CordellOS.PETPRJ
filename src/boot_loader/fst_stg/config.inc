%ifndef _CONFIG__INC_
%define _CONFIG__INC_

; fst_stg will relocate itself to this address
; make sure to leave room for the stack which will grow downward
; don't forget to update the offset in linker.ld as well

FST_STG_SEGMENT              equ 0x0
FST_STG_OFFSET               equ 0x700

%endif