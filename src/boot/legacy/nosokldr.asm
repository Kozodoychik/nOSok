
extern ldrmain

global _start


global bios_get_vesa_modeinfo
global bios_set_vesa_mode
global bios_get_edid

global bios_drive_read

global boot_drive

bits 16
section .text
_start:
    mov [boot_drive], dl

    cli
    cld

    mov ax, 0x7c00
    mov sp, ax

    in al, 0x92
    or al, 2
    out 0x92, al

    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:pmode_start

bits 32
pmode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    call ldrmain

    hlt
    jmp $


; Великие функции, которые заставляют процессор "скакать" между режимами

bios_get_vesa_modeinfo:
    push ebp
    mov ebp, esp
    pusha

    call set_rmode
    bits 16

    mov di, [ebp+12]
    mov cx, [ebp+8]
    mov ax, 0x4f01
    int 10h

    call set_pmode
    bits 32
    
    popa
    pop ebp
    ret

bios_set_vesa_mode:
    push ebp
    mov ebp, esp

    call set_rmode
    bits 16

    mov ax, 0x4f02
    mov bx, [ebp+8]
    int 10h

    call set_pmode
    bits 32

    pop ebp
    ret

bios_get_edid:
    push ebp
    mov ebp, esp

    call set_rmode
    bits 16

    mov ax, 0x4f15
    mov bx, 1
    xor cx, cx
    xor dx, dx
    mov di, [ebp+8]
    int 10h

    call set_pmode
    bits 32

    pop ebp
    ret

bios_drive_read:
    push ebp
    mov ebp, esp
    pusha

    call set_rmode
    bits 16

    mov ax, [ebp+8]
    call lba2chs

    push es
    mov ch, al
    mov dh, bl
    mov dl, [boot_drive]
    mov bx, [ebp+16]
    mov ax, [ebp+18]
    shl ax, 12
    mov es, ax
    mov al, [ebp+12]
    mov ah, 02h
    int 13h
    pop es

    call set_pmode
    bits 32

    popa
    pop ebp
    ret

; ВХОД: ax - LBA
; ВЫХОД: ax - C, bx - H, cx - S
bits 16
lba2chs:
    mov cx, [0x7c18] ; secs_per_track
    xor dx, dx
    div cx
    mov cx, dx
    inc cx
    mov bx, [0x7c1a] ; head_count
    xor dx, dx
    div bx
    mov bx, dx
    ret


; А это великие функции переключения между режимами

; set_rmode - переключение в реальный режим
bits 32
set_rmode:
    lgdt [gdtr16]
    jmp 0x08:.pmode_16

    bits 16
    .pmode_16:
        cli

        mov eax, cr0
        and al, 0xfe
        mov cr0, eax

        jmp 0x00:.rmode

    .rmode:
        xor ax, ax
        mov es, ax
        mov ds, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax

        sti
        ret

; set_pmode - переключение в защищённый режим
bits 16
set_pmode:
    cli

    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:.pmode

    bits 32
    .pmode:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax

        ret


boot_drive: db 0

gdt_start:
    dq 0

    gdt_code:
        dw 0xffff
        dw 0
        db 0
        db 0x9a
        db 0xcf
        db 0

    gdt_data:
        dw 0xffff
        dw 0
        db 0
        db 0x92
        db 0xcf
        db 0
gdt_end:

gdt16_start:
    dq 0

    gdt_code16:
        dw 0xffff
        dw 0
        db 0
        db 0x9a
        db 0x0f
        db 0

    gdt_data16:
        dw 0xffff
        dw 0
        db 0
        db 0x92
        db 0x0f
        db 0
gdt16_end:

gdtr:
    dw (gdt_end - gdt_start) - 1
    dd gdt_start

gdtr16:
    dw (gdt16_end - gdt16_start) - 1
    dd gdt16_start
