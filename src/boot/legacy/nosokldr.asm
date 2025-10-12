%include "src/boot/legacy/addrs.inc"

bits 16
org 0x4000
section .text

; Загружаем ядро
_start:
    mov [bootdev], dl
    mov [first_data_sect], cx

    mov ax, ROOT_DIR_ADDR_SEG
    mov es, ax

    mov si, welcome_msg
    call print

    xor bx, bx
    mov di, bx

    .find_loop:
        mov cx, 11
        mov di, bx
        mov si, kernel_file

        cmp byte [es:di], 0
        je .not_found

        repe cmpsb
        jz .found

        add bx, 32
        jmp .find_loop

    .not_found:
        mov si, kernel_not_found_msg
        call print
        jmp $

    .found:
        mov di, bx
        xor bx, bx
        add di, 1ah

        .loop:
            xor dx, dx
            mov ax, [es:di]
            cmp ax, 0xfff8
            
            jae .stop
            sub ax, 2
            xor cx, cx
            mov cl, [0x7c0d]     ; secs_per_cluster
            xor dx, dx
            mul cx

            add ax, [first_data_sect]

            push es
            push bx
            mov bx, KERNEL_FILE_SEG
            mov es, bx
            pop bx

            call disk_read
            pop es

            mov ax, [0x7c0b]     ; bytes_per_sector
            mov cx, [0x7c0d]     ; secs_per_cluster
            xor dx, dx
            mul cx
            add bx, ax
            
            
            mov ax, [es:di]
            mov cx, FAT_ADDR_SEG
            mov es, cx
            shl ax, 1
            mov di, ax

            jmp .loop

        .stop:
        xor ax, ax
        mov es, ax

        call setup_edid
        call setup_video

        jmp setup_pmode

    jmp $


; ВХОД: ax - LBA
; ВЫХОД: ax - C, bx - H, cx - S
lba2chs:
    mov cx, [0x7c18]     ; secs_per_track
    xor dx, dx
    div cx
    mov cx, dx
    inc cx
    mov bx, [0x7c1a]     ; head_count
    xor dx, dx
    div bx
    mov bx, dx
    ret

; ВХОД: ax - LBA, es:bx - буффер, cx - кол-во секторов
disk_read:
    pusha
    push bx
    push cx
    call lba2chs
    mov dh, bl
    mov ch, al
    pop ax
    pop bx
    mov ah, 02h
    mov dl, [bootdev]
    int 13h
    popa
    ret

print:
    mov ah, 0eh
    .loop:
        mov al, [si]
        test al, al
        jz .ret
        int 10h
        inc si
        jmp .loop
    .ret:
        ret

bootdev: db 0
first_data_sect: dw 0

; НЕ ПРОВЕРЕНО!!! Виртуальные машины не поддерживают EDID/DDC
setup_edid:
    mov ax, 0x4f15
    mov bx, 1
    xor cx, cx
    xor dx, dx
    mov di, edid
    int 10h

    cmp ax, 0x004f
    jne .not_supported

    test cx, 1
    jz .not_supported

    mov al, [edid+0x38]
    mov ah, [edid+0x3a]
    shr ah, 4
    mov [display_mode.width], ax

    mov al, [edid+0x3b]
    mov ah, [edid+0x3d]
    shr ah, 4
    mov [display_mode.height], ax

    jmp .ret

    .not_supported:
        mov si, ddc_not_supported
        call print

    .ret:
        ret

; Поиск нужного режима среди режимов VESA
setup_video:
    xor cx, cx
    .loop:
        clc
        mov ax, 0x4f01
        mov di, vesa_modeinfo
        int 10h
        jc .skip
        cmp ax, 0x004f
        jne .skip

        mov ax, [vesa_modeinfo.attrib]
        test ax, 0x90
        jz .skip

        mov al, [vesa_modeinfo.memory_model]
        cmp al, 4
        je ._
        cmp al, 6
        jne .skip
        
    ._:
        mov ax, [vesa_modeinfo.width]
        cmp ax, [display_mode.width]
        jne .skip

        mov ax, [vesa_modeinfo.height]
        cmp ax, [display_mode.height]
        jne .skip

        mov bx, cx
        mov ax, 0x4f02
        int 10h

        mov eax, [vesa_modeinfo.fb]
        mov dword [display_mode.fb], eax
        xor eax, eax

        jmp .ret

    .skip:
        cmp cx, 0xffff
        je .ret
        inc cx
        jmp .loop

    .not_supported:
        mov si, vbe_not_supported
        call print
    .ret:
        ret


; Переходим в защищённый режим
setup_pmode:
    cli

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


    hlt
    jmp $

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

gdtr:
    dw (gdt_end - gdt_start) - 1
    dd gdt_start

welcome_msg: db "Welcome to nOSok BootLoader!", 0x0a, 0x0d, 0
kernel_not_found_msg: db "/KERNEL not found", 0x0a, 0x0d, 0
ddc_not_supported: db "VESA DDC not supported", 0x0a, 0x0d, 0
vbe_not_supported: db "VBE not supported", 0x0a, 0x0d, 0

kernel_file: db "KERNEL     "

display_mode:
    .width:     dw 800
    .height:    dw 600
    .bpp:       db 24
    .fb:        dd 0

section .bss
vesa_modeinfo:
    .attrib:                resw 1
    .win1_attrib:           resb 1
    .win2_attrib:           resb 1
    .win_gran:              resw 1
    .win_size:              resw 1
    .win1_seg:              resw 1
    .win2_seg:              resw 1
    .win_func_ptr:          resd 1
    .bytes_per_scanline:    resw 1
    .width:                 resw 1
    .height:                resw 1
    .char_width:            resb 1
    .char_height:           resb 1
    .planes:                resb 1
    .bpp:                   resb 1
    .banks:                 resb 1
    .memory_model:          resb 1
    .bank_size:             resb 1
    .image_pages:           resb 1
    resb 1
    .red_mask_size:         resb 1
    .red_field_pos:         resb 1
    .green_mask_size:       resb 1
    .green_field_pos:       resb 1
    .blue_mask_size:        resb 1
    .blue_field_pos:        resb 1
    .res_mask_size:         resb 1
    .res_mask_pos:          resb 1
    .direct_color_modeinfo: resb 1

    .fb:                    resd 1
    .offscreen_offs:        resd 1
    .offscreen_size:        resw 1
    resb 206

edid:
    resb 128