bits 16


global dev_write
dev_write:
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

global dev_write_word
dev_write_word:
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

global dev_read
dev_read:
	push edx

	xor edx, edx
	xor eax, eax

	mov dx, [esp + 8]

	in ax, dx

	pop edx

	ret