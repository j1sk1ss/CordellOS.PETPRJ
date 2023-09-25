bits 32


global x86_outb
x86_outb:

	push edx
	push eax
	
	xor edx, edx
	xor eax, eax
	
	mov dx, [esp + 12]
	mov al, [esp + 16]
	
	out dx, al 
	
	pop eax
	pop edx
	
	ret

global x86_outw
x86_outw:
    
	push edx
	push eax
	
	xor edx, edx
	xor eax, eax
	
	mov dx, [esp + 12]
	mov ax, [esp + 16]
	
	out dx, ax 
	
	pop eax
	pop edx
	
	ret

global x86_inb
x86_inb:
    
	push edx
	
	xor edx, edx
	xor eax, eax
	
	mov dx, [esp + 8]
	
	in ax, dx
	
	;jmp $
	
	pop edx
	
	ret