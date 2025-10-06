ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 1M;

    _kernel_start = .;
    
    .boot : {
        KEEP(*(.multiboot_header))
    }

    .boot.text : {
        *(.boot.text)
    }

    .boot.bss : {
        *(.boot.bss)
    }

    . += 0xC0000000;

    .text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000) {
        *(.text)
    }

    .bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000) {
        *(COMMON)
        *(.bss)
    }

    . -= 0xC0000000;

    _kernel_end = .;
}