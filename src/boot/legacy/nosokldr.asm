bits 16
org 0x40000

_start:
    mov ax, 0x4000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov si, welcome_msg
    call print

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

welcome_msg: db "Welcome to nOSok BootLoader!", 0x0a, 0x0d, 0