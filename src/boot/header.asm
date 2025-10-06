section .multiboot_header

dd 0x1badb002
dd (1 << 0) | (1 << 1)
dd -(0x1badb002 + ((1 << 0) | (1 << 1)))