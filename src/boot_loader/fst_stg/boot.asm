org 0x7C00 
bits 16

;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;

%define ENDL 0x0D, 0x0A


;
; FAT12 header
;

	jmp short boot
	nop

	bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
	bdb_bytes_per_sector:       dw 512
	bdb_sectors_per_cluster:    db 1
	bdb_reserved_sectors:       dw 1
	bdb_fat_count:              db 2
	bdb_dir_entries_count:      dw 0E0h
	bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
	bdb_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
	bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
	bdb_sectors_per_track:      dw 18
	bdb_heads:                  dw 2
	bdb_hidden_sectors:         dd 0
	bdb_large_sector_count:     dd 0

	; extended boot record
	ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
								db 0                    ; reserved
	ebr_signature:              db 29h
	ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
	ebr_volume_label:           db 'CORDELL  OS'        ; 11 bytes, padded with spaces
	ebr_system_id:              db 'FAT12   '           ; 8 bytes


times 90-($-$$) db 0


;
;	Code goes here
;

	boot:
		mov ax, 0				; setup data
		mov ds, ax
		mov es, ax
								; setup stack
		mov ss, ax
		mov sp, 0x7C00 			; stack go down from place, where we load

								; Some BIOS'es can start on 07C0:0000
		push es					;
		push word .after		;
		retf					; Long return

	.after:
		; read something from floppy
		; BIOS should set DL to drive number
		mov [ebr_drive_number], dl

		; show loading message
		mov si, msg_loading
		call print

		; check instentions present
		mov ah, 0x41
		mov bx, 0x55AA
		stc
		int 13h

		jc .no_disk_extensions
		cmp bx, 0xAA55
		jne .no_disk_extensions

		; extention present
		mov byte [have_extension], 1
		jmp .after_disk_extensions_check

	.no_disk_extensions:
    	mov byte [have_extension], 0

	.after_disk_extensions_check:
		; load sce_stg
		mov si, sec_stg_location

		mov ax, SEC_STAGE_LOAD_SEGMENT         ; set segment registers
		mov es, ax

		mov bx, SEC_STAGE_LOAD_OFFSET

	.loop:
		mov eax, [si]
		add si, 4
		mov cl, [si]
		inc si

		cmp eax, 0
		je .read_finish

		call read_disk_data

		xor ch, ch
		shl cx, 5
		mov di, es
		add di, cx
		mov es, di

		jmp .loop

	.read_finish:
		
		; jump to our kernel
		mov dl, [ebr_drive_number]          ; boot device in dl

		mov ax, SEC_STAGE_LOAD_SEGMENT         ; set segment registers
		mov ds, ax
		mov es, ax

		jmp SEC_STAGE_LOAD_SEGMENT:SEC_STAGE_LOAD_OFFSET

		jmp wait_key_end_reboot             ; should never happen

		cli                                 ; disable interrupts, this way CPU can't get out of "halt" state
		hlt


	;
	;	Error handlers
	;

	floppy_error:
		mov si, msg_reading_fail
		call print
		jmp wait_key_end_reboot

	sec_stg_not_found_error:
		mov si, msg_sec_stg_not_found
		call print
		jmp wait_key_end_reboot

	wait_key_end_reboot:
		mov ah, 0
		int 16h						; wait for kyboard press
		jmp 0FFFFh:0				; jump to begining of BIOS

	.halt:							; disable interrupts, this way we can't get out of "halt" state
		cli
		hlt

	;
	;	Prints a char array to the screen
	;	Params:
	;		- ds:si points to string

	print:
		;	save registers we will modify
		push si
		push ax
		push bx

	.loop:
		lodsb     		; load next char rin al
		or al, al 		; verify next char for null status
		jz .done

		mov ah, 0x0E	; call bios text interrupt
		mov bh, 0		; set page to zero
		int 0x10		; call Video interrapt

		jmp .loop

		.done: 				;	Loop done and we 
			pop bx
			pop ax
			pop si

			ret


	;
	;	Disk functions
	;

	;
	;	Converts an LBA address to a CHS address
	;	Params:
	;		- ax: LBA address
	;	Ret:
	;		- cx: [bits 0 - 5]: sector number
	;		- cx: [bits 6 - 15]: cylinder
	;		- dh: head
	;

	convert_lba_to_chs:
		push ax
		push dx

		xor dx, dx							; dx = 0
		div word [bdb_sectors_per_track]	; ax = LBA / SectorsPerTrack
											; dx = LBA % SectorsPerTrack
		inc dx								; dx = (LBA % SectorsPerTrack + 1) = sector
		mov cx, dx

		xor dx, dx  						; dx = 0 
		div word [bdb_heads]				; dx = 0
											; ax = (LBA / SectorsPerTrack) / Heads = cylinder
											; dx = (LBA / SectorsPerTrack) % Heads = head
		mov dh, dl							; dh = head
		mov ch, al							; ch = cylinder (lower 8 bits)
		shl ah, 6
		or cl, ah							; put upper 2 bits of cylinder in cl

		pop ax								; get changed value
		mov dl, al							; return value to dl
		pop ax								; clean stack

		ret


	;
	;	Disk read function (read sectors)
	;	Params:
	;		- eax: LBA address
	;		- cl: number of sectors to read (max is 128)
	;		- dl: drive number
	;		- es & bx: memory address for data from disk
	;

	read_disk_data:
		push eax							; Save register that will be modify
		push bx
		push cx
		push dx
		push si
		push di

		cmp byte [have_extension], 1
		jne .no_disk_extention

		; with extention
		mov [extension_struct.lba], eax
		mov [extension_struct.segment], es
		mov [extension_struct.offset], bx
		mov [extension_struct.count], cl

		mov ah, 0x42
		mov si, extension_struct
		mov di, 3

		jmp .retry

		.no_disk_extention:
			push cx								; Save cl (number of sectors for reading)
			call convert_lba_to_chs				; Converting
			pop ax								; AL = num of sectors for read

			mov ah, 02h
			mov di, 3							; retry count

		.retry:
			pusha							; save all registers, cuz we don't know what bios modify
			stc								; set carry flag (Some bios'es don't set him)
			int 13h							; if flag cleared -> success
			jnc .done

			; reading failed
			popa
			call disk_reset

			dec di
			test di, di
			jnz .retry
			
			.fail:
				; after all attemps
				jmp floppy_error

			.done:
				popa

				pop di
				pop si
				pop dx
				pop cx
				pop bx
				pop eax

				ret

	
	;
	;	Disk restore function
	;	Params:
	;		- dl: drive number
	;

	disk_reset:
		pusha 			; push general registers to stack
		mov ah, 0
		stc
		int 13h
		jc floppy_error
		popa			; pop general registers from stack

		ret


msg_loading: 			db 'Loading...', ENDL, 0
msg_reading_fail: 		db 'Read was failed', ENDL, 0
msg_sec_stg_not_found:	db 'Sec stage not found!', ENDL, 0	
file_sec_stg_bin:		db 'SEC_STG BIN' 								; Don't foget 11 bytes name	
have_extension:			db 0

extension_struct:
	.size:				db 10h											; Reserved struct of extention 
						db 0
	.count:				dw 0
	.offset:			dw 0
	.segment:			dw 0
	.lba:				dq 0

SEC_STAGE_LOAD_SEGMENT	equ 0x0
SEC_STAGE_LOAD_OFFSET	equ 0x500

times 510-30-($-$$) 	db 0
sec_stg_location:		times 30 db 0									; Generate 30 bytes with 0
						dw 0AA55h	
buffer: