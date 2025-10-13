%include "src/boot/legacy/addrs.inc"

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
secs_per_track:     dw 32
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

    ; Крайне бесполезный код для ВМ, но нужный для работы на реальном железе
    ; Сегментные регистры на реальных системах могут не подчищаться перед прыжком в загрузчик
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax

    ; Прикол: геометрия диска в BIOS почти всегда не совпадает со значениями в BPB
    ; Так что получаем эти значения и используем уже их
    mov ah, 08h
    int 13h
    inc dh
    mov byte [head_count], dh
    and cl, 0x3f
    mov byte [secs_per_track], cl

    ; Читаем FAT
    mov ax, [reserved_secs]
    mov bx, FAT_ADDR_SEG
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
        add bx, 32
        jmp .find_loop

    ; Не нашли
    .not_found:
        xor ax, ax
        mov ds, ax
        mov es, ax
        lea si, [not_found_msg]
        call print
        jmp $

    ; Нашли
    .found:
        mov si, bx
        
        mov ax, ds
        mov es, ax
        xor ax, ax
        mov ds, ax

        mov al, [secs_per_cluster]
        
        mov cx, [es:si+1ah]
    
        shl cx, 1
        xor si, si
        add si, cx
        shr cx, 1

        sub cx, 2
        xor dx, dx
        mul cx

        xor bx, bx
        mov ds, bx
        mov es, bx

        mov bx, NOSOKLDR_ADDR
        
        ; Загружаем кластеры в память
        .loop:
            add ax, [first_data_sect]
            xor cx, cx
            mov cl, [secs_per_cluster]
            ;jmp $
            call disk_read
            cmp word [si], 0xfff8
            jge .stop
            
            xor ax, ax
            mov al, [secs_per_cluster]
            mul word [bytes_per_sector]
            add bx, ax
            xor ax, ax
            mov al, [secs_per_cluster]


            mov dx, FAT_ADDR_SEG
            mov ds, dx
            mov cx, [si]
            xor dx, dx
            mov ds, dx

            shl cx, 1
            mov si, cx
            shr cx, 1

            sub cx, 2
            xor dx, dx
            mul cx

            mov cx, [secs_per_cluster]

            jmp .loop

        ; Прыгаем в загрузчик
        .stop:
            mov dl, [bootdev]
            mov cx, [first_data_sect]
            jmp NOSOKLDR_ADDR


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
    xor ah, ah
    xor bx, bx
    mov ds, bx
    call lba2chs
    mov dh, bl
    mov ch, al
    pop ax
    pop bx
    mov ah, 02h
    mov dl, [bootdev]
    int 13h
    jc .error
    ret

    .error:
        xor bx, bx
        mov ds, bx
        mov si, disk_read_err
        call print
        mov al, ah
        xor ah, ah
        call print_hex
        jmp $

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

print_hex:
    pusha
    shl ax, 4
    shr al, 4
    xchg ah, al
    mov bx, hex_digits
    xlat
    push ax
    mov ah, 0eh
    int 10h
    pop ax
    xchg ah, al
    xlat
    mov ah, 0eh
    int 10h
    popa
    ret

bootdev: db 0
first_data_sect: dw 0

hex_digits: db "0123456789ABCDEF"
stage2_file: db "NOSOKLDRBIN"
not_found_msg: db "/NOSOKLDR.BIN not found", 0ah, 0dh, 0
disk_read_err: db "Disk error ", 0
times 510-($-$$) db 0
db 0x55, 0xaa