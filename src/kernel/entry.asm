section .multiboot
align 4
    dd 0x1badb002
    dd (1 << 0) | (1 << 1)
    dd -(0x1badb002 + ((1 << 0) | (1 << 1)))

section .boot.text

extern _kernel_start
extern _kernel_end

global _start

global stack_end
global page_directory

extern kmain

; Включаем страничную память (paging) и отображаем ядро в 0xC0000000

_start:
    mov edi, page_table_0

    mov esi, 0
    mov ecx, 1023

    .loop:
        ;cmp esi, _kernel_start
        ;jl .skip
        ;cmp esi, _kernel_end
        ;jge .end

        mov eax, esi
        add eax, 0x3
        mov [edi], eax

    .skip:
        add esi, 4096
        add edi, 4
        loop .loop

    .end:
    mov dword [(page_table_0) + (1023 * 4)], 0xB8003 

    mov eax, page_table_0 + 0x3

    mov [page_directory], eax
    mov [page_directory + 768 * 4], eax

    mov eax, page_directory
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80010000
    mov cr0, eax

    mov ecx, _k_start
    jmp ecx

section .text

_k_start:
    mov esp, stack_end

    call kmain

    cli
    hlt

section .boot.bss nobits align=4096
page_directory:
    resb 4096
page_table_0:
    resb 4096

section .bss
stack_start:
resb 1024 * 1024
stack_end: