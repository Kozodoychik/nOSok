FAT_ADDR            equ 0x8000
ROOT_DIR_ADDR_SEG   equ 0x2000
NOSOKLDR_ADDR_SEG   equ 0xc000

bits 16
org 0x7c00

jmp short start
nop

; BPB
; Некоторые значения взяты с пустого 32 МБ образа, отформатированного через mkfs.fat
oem_id:             db "nOSok   "
bytes_per_sector:   dw 512
secs_per_cluster:   db 4
reserved_secs:      dw 4
fat_count:          db 2
root_entry_count:   dw 512
secs_count:         dw 65535
media_descr:        db 0xf8
secs_per_fat:       dw 64
secs_per_track:     dw 63   ; QEMU почему-то считает, что в дорожке 63 сектора, в отличии от 32 в образе mkfs.fat
head_count:         dw 64

hidden_secs:        dd 0
large_sect_count:   dd 0

; EBR

drive_num:          db 0x80
nt_flags:           db 0
signature:          db 0x29
volume_id:          dd 0x12345678
volume_label:       db "nOSok boot "
sys_id:             db "FAT16   "

start:
    mov [bootdev], dl

    ; Читаем FAT
    mov ax, [reserved_secs]
    mov bx, FAT_ADDR
    mov es, bx
    xor bx, bx
    mov cx, [secs_per_fat]
    call disk_read


    xor dx, dx
    xor ax, ax

    ; Считаем начальный сектор корневой директории    
    mov al, [fat_count]
    mul word [secs_per_fat]
    add ax, [reserved_secs]

    xchg ax, cx

    ; Считаем размер корневой директории в секторах
    mov ax, [root_entry_count]
    mov bx, 32
    mul bx
    mov bx, [bytes_per_sector]
    div bx
    xchg ax, cx

    push ax
    add ax, cx
    mov [first_data_sect], ax
    pop ax

    ; Читаем корневую директорию
    mov bx, ROOT_DIR_ADDR_SEG
    mov es, bx
    xor bx, bx
    call disk_read

    mov ax, ROOT_DIR_ADDR_SEG
    mov ds, ax

    xor ax, ax
    mov es, ax

    xor bx, bx
    ; Ищем файл
    xor si, si
    mov di, stage2_file


    .find_loop:
        mov cx, 11
        mov si, bx
        cmp byte [si], 0
        je .not_found
        mov di, stage2_file
        repe cmpsb
        jz .found
    .skip:
        add bx, 32
        jmp .find_loop

    .not_found:
        xor ax, ax
        mov ds, ax
        mov es, ax
        lea si, [not_found_msg]
        call print
        jmp $

    .found:
        mov ah,0eh
        mov al,'F'
        int 10h
        mov bx, 0xffff
        jmp $


; ВХОД: ax - LBA
; ВЫХОД: ax - C, bx - H, cx - S
lba2chs:
    mov cx, [secs_per_track]
    xor dx, dx
    div cx
    mov cx, dx
    inc cx
    mov bx, [head_count]
    xor dx, dx
    div bx
    mov bx, dx
    ret

; ВХОД: ax - LBA, es:bx - буффер, cx - кол-во секторов
disk_read:
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
    ret

print:
    push si
    mov ah, 0eh
    .loop:
        mov al, [si]
        test al, al
        jz .ret
        int 10h
        inc si
        jmp .loop
    .ret:
        pop si
        ret

bootdev: db 0
first_data_sect: dw 0

stage2_file: db "NOSOKLDRBIN"
not_found_msg: db "NOSOKLDR.BIN not found", 0ah, 0dh, 0
times 510-($-$$) db 0
db 0x55, 0xaa