;section .text_read
global x86_disk_read

x86_disk_read:
    ; Input: AL = drive, CX = cylinder, DX = sector, BX = head, SI = count, DI = data pointer
    ; Output: EAX = 1 on success, 0 on failure

    ; Set up segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    ; ATA command ports
    mov dx, 0x1F7  ; Command port
    mov al, 0x20   ; ATA READ SECTORS command
    out dx, al

    ; Wait for the drive to become ready
    .wait_ready:
        in al, dx
        test al, 0x40
        jnz .wait_ready_failure  ; Jump to failure label if not ready

    ; Set up parameters: sector count, cylinder, sector number, head
    mov ax, si     ; Number of sectors to read
    mov dx, 0x1F2  ; Sector count port
    out dx, al

    mov dx, 0x1F3  ; Sector number port
    mov al, dl     ; Sector number
    out dx, al

    mov dx, 0x1F4  ; Cylinder low port
    mov al, cl     ; Low byte of cylinder
    out dx, al

    mov dx, 0x1F5  ; Cylinder high port
    mov al, ch     ; High byte of cylinder
    out dx, al

    mov dx, 0x1F6  ; Drive and head port
    mov al, 0xE0   ; Use the LBA mode, set highest bit
    or  al, byte [edi]  ; Set drive number
    out dx, al

    ; Command execution
    mov dx, 0x1F7  ; Command port
    mov al, 0x30   ; Execute the command
    out dx, al

    ; Poll until the busy flag is clear
    in al, dx
    test al, 0x80
    jnz .wait_ready_failure  ; Jump to failure label if still busy

    ; Read data from the data port
    mov dx, 0x1F0  ; Data port
    mov cx, si     ; Number of words to read
    rep insw       ; Read data from data port to memory

    ; Set return value based on success
    mov eax, 1      ; Set EAX to 1 for success
    ret

.wait_ready_failure:
    ; Set return value to 0 for failure
    mov eax, 0
    ret

;section .text_reset
global x86_disk_reset

x86_disk_reset:
    ; Input: AL = drive
    ; Output: None

    ; Set up segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    ; ATA command ports
    mov dx, 0x1F7  ; Command port
    mov al, 0x04   ; ATA RESET command
    out dx, al

    ; Wait for the drive to become ready
    .wait_ready_reset:
        in al, dx
        test al, 0x40
        jnz .wait_ready_reset

    ret