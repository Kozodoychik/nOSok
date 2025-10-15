ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS {
    
    . = 0x4000;

    .text : {
        *(.text)
    }

    .bss : {
        *(.bss)
    }

}