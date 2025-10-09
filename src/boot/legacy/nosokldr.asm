bits 16
org 0xc000:0000

_start:
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
        return

welcome_msg: db "Welcome to nOSok BootLoader!", 0x0a, 0x0d, 0