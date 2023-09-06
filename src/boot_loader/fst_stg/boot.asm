;
;   Cordell OS base code
;   Cordell OS is simple example of Operation system from scratch
;

org 0x7C00 
bits 16

%define ENDL 0x0D, 0x0A ; Macross for printing text


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
		push es
		push word .after
		retf					; Long return

	.after:
		; read something from floppy
		; BIOS should set DL to drive number
		mov [ebr_drive_number], dl

		; show loading message
		mov si, msg_loading
		call print

		; read drive params
		push es
		mov ah, 08h
		int 13h
		jc floppy_error
		pop es

		and cl, 0x3F						; remove top 2 bites
		xor ch, ch
		mov [bdb_sectors_per_track], cx		; sector count

		inc dh
		mov [bdb_heads], dh					; head count

		; Read FAT root directory
		mov ax, [bdb_sectors_per_fat]		; compute LMA of root directory = reserved + fats * sectors_per_fat
		mov bl, [bdb_fat_count]
		xor bh, bh
		mul bx								; ax = (fats * sec_per_fat)
		add ax, [bdb_reserved_sectors]		; ax = LBA of root dir
		push ax
		
		; compute size of root directory = (32 * number_of_entries) / bytes_per_sector
		mov ax, [bdb_dir_entries_count]
		shl ax, 5							; ax *= 32
		xor dx, dx							; dx = 0
		div word [bdb_bytes_per_sector]		; num of sectors we need to read

		test dx, dx							; if dx != 0, add 1
		jz .root_dir_after
		inc ax								; != 0, add 1

		.root_dir_after:
			mov cl, al							; cl = num of sectors to read
			pop ax								; ax = LBA of root directory
			mov dl, [ebr_drive_number]			; dl = drive num
			mov bx, buffer						; es & bx buffer
			call read_disk_data

			; Search kernel.bin
			xor bx, bx
			mov di, buffer

	.search_kernel:
		mov si, file_kernel_bin
		mov cx, 11							; compare up to 11 bytes
		push di
		repe cmpsb
		pop di
		je .found_kernel

		add di, 32
		inc bx
		cmp bx, [bdb_dir_entries_count]
		jl .search_kernel

		; kernel not found
		jmp kernel_not_found_error

	.found_kernel:
		; di should have address
		mov ax, [di + 26]					; first logical cluster field (offset is 26)
		mov [kernel_cluster], ax

		; load FAT from disk to memory
		mov ax, [bdb_reserved_sectors]
		mov bx, buffer
		mov cl, [bdb_sectors_per_fat]
		mov dl, [ebr_drive_number]
		call read_disk_data

		; read kernel and process FAT chain
		mov bx, SEC_STAGE_LOAD_SEGMENT
		mov es, bx
		mov bx, SEC_STAGE_LOAD_OFFSET

	.load_kernel_loop:
		mov ax, [kernel_cluster]
		add ax, 31 							; hardcoded number. Correctly works with floppy disks
		mov cl, 1
		mov dl, [ebr_drive_number]
		call read_disk_data

		add bx, [bdb_bytes_per_sector]

		; compute location of next cluster
		mov ax, [kernel_cluster]
		mov cx, 3
		mul cx
		mov cx, 2
		div cx								; ax = index of entry in FAT, dx = cluster % 2

		mov si, buffer
		add si, ax
		mov ax, [ds:si]

		or dx, dx
		jz .even

		.odd:
			shr ax, 4
			jmp .next_cluster_after
		
		.even:
			and ax, 0x0FFF

			.next_cluster_after:
				cmp ax, 0x0FF8
				jae .read_finish

				mov [kernel_cluster], ax
				jmp .load_kernel_loop

				.read_finish:
					; boot device in dl
					mov dl, [ebr_drive_number]

					; set segment registers
					mov ax, SEC_STAGE_LOAD_SEGMENT
					mov ds, ax
					mov es, ax

					jmp SEC_STAGE_LOAD_SEGMENT:SEC_STAGE_LOAD_OFFSET
					jmp wait_key_end_reboot 


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
	;	Error handlers
	;

	floppy_error:
		mov si, msg_reading_fail
		call print
		jmp wait_key_end_reboot

	kernel_not_found_error:
		mov si, msg_kernel_not_found
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
	;		- ax: LBA address
	;		- cl: number of sectors to read (max is 128)
	;		- dl: drive number
	;		- es & bx: memory address for data from disk
	;

	read_disk_data:
		push ax								; Save register that will be modify
		push bx
		push cx
		push dx
		push di

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
				pop dx
				pop cx
				pop bx
				pop ax

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
msg_kernel_not_found:	db 'Second stage not found!', ENDL, 0	
file_kernel_bin:		db 'SEC_STG BIN' ; Don't foget 11 bytes name	

kernel_cluster:			dw 0

SEC_STAGE_LOAD_SEGMENT		equ 0x0
SEC_STAGE_LOAD_OFFSET		equ 0x500

times 510-($-$$) db 0
dw 0AA55h	

buffer: