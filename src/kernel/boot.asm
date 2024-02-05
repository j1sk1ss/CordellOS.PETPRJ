; Some code taked from https://forum.osdev.org/viewtopic.php?f=1&t=28429

; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            		  ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            		  ; provide memory map
VBEMODE  equ  1 << 2 					  ; flag for VBE support
MBFLAGS  equ  MBALIGN | MEMINFO | VBEMODE ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        		  ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + MBFLAGS) 		  ; checksum of above, to prove we are multiboot
 
HEADER_TAG_FB        equ 0x5
HEADER_TAG_OPTIONAL  equ 0x1

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.

; ============================
;	MB STRUCT HEADER
; ============================

	section .multiboot
	align 4

	mb_header_start:
		dd MAGIC
		dd MBFLAGS
		dd CHECKSUM

		frame_buffer_start:
			dd 0, 0, 0, 0, 0
			dd 1  ; 1 - text mode, 0 - VGA mode
			dd 1024, 768, 32
		frame_buffer_end:

	mb_header_end:
 
; ============================
;	MB STRUCT HEADER
; ============================

; The multiboot standard does not define the value of the stack pointer register
; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; small stack by creating a symbol at the bottom of it, then allocating 16384
; bytes for it, and finally creating a symbol at the top. The stack grows
; downwards on x86. The stack is in its own section so it can be marked nobits,
; which means the kernel file is smaller because it does not contain an
; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; System V ABI standard and de-facto extensions. The compiler will assume the
; stack is properly aligned and failure to align the stack will result in
; undefined behavior.

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 
; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.

section .text
global _start:function (_start.end - _start)
_start:

	; The bootloader has loaded us into 32-bit protected mode on a x86
	; machine. Interrupts are disabled. Paging is disabled. The processor
	; state is as defined in the multiboot standard. The kernel has full
	; control of the CPU. The kernel can only make use of hardware features
	; and any code it provides as part of itself. There's no kprintf
	; function, unless the kernel provides its own <stdio.h> header and a
	; kprintf implementation. There are no security restrictions, no
	; safeguards, no debugging mechanisms, only what the kernel provides
	; itself. It has absolute and complete power over the
	; machine.
 
	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems). This is necessarily done
	; in assembly as languages such as C cannot function without a stack.

	mov esp, stack_top
 
	; This is a good place to initialize crucial processor state before the
	; high-level kernel is entered. It's best to minimize the early
	; environment where crucial features are offline. Note that the
	; processor is not fully initialized yet: Features such as floating
	; point instructions and instruction set extensions are not initialized
	; yet. The GDT should be loaded here. Paging should be enabled here.
	; C++ features such as global constructors and exceptions will require
	; runtime support to work as well.
 
	; Enter the high-level kernel. The ABI requires the stack is 16-byte
	; aligned at the time of the call instruction (which afterwards pushes
	; the return pointer of size 4 bytes). The stack was originally 16-byte
	; aligned above and we've since pushed a multiple of 16 bytes to the
	; stack since (pushed 0 bytes so far) and the alignment is thus
	; preserved and the call is well defined.
    ; note, that if you are building on Windows, C functions may have "_" prefix in assembly: _kernel_main

	extern kernel_main
	call kernel_main
 
	cli
.hang:	hlt
	jmp .hang
.end: