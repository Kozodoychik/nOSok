#include <stdint.h>
#include <elf.h>
#include <std/string.h>
#include <boot/vesa.h>


void setup_display() {
    vesa_modeinfo mode;
    edid_record edid;

    bios_get_edid(&edid);

    if (edid.padding != 0x00ffffffffffff00) {
        bios_set_vesa_mode(0x4115);
        return;
    }

    uint16_t width = edid.timing_descr1.h_active_time | ((edid.timing_descr1.h_active_blank_time & 0xf0) << 4);
    uint16_t height = edid.timing_descr1.v_active_time | ((edid.timing_descr1.v_active_blank_time & 0xf0) << 4);

    for (uint16_t m = 0x4100; m < 0xffff; m++) {
        bios_get_vesa_modeinfo(m, &mode);

        if (mode.width == width && mode.height == height && mode.bpp == 32) {
            bios_set_vesa_mode(m);
            return;
        }
    }

    bios_set_vesa_mode(0x4115);
}

void ldrmain(void* kernel_file) {

    setup_display();

    // Получаем заголовок в самом начале файла
    Elf32_Ehdr* elf_header = (Elf32_Ehdr*)(kernel_file);

    // Проверка идентификации и некоторых параметров
    if (elf_header->e_ident[0] != ELFMAG0 || 
        elf_header->e_ident[1] != ELFMAG1 ||
        elf_header->e_ident[2] != ELFMAG2 ||
        elf_header->e_ident[3] != ELFMAG3 ||
        
        elf_header->e_version != EV_CURRENT ||

        elf_header->e_machine != EM_386 ||
    
        elf_header->e_type != ET_EXEC){
            return;
    }

    // Получаем адрес таблицы заголовков программы
    Elf32_Phdr* elf_phdr = (Elf32_Phdr*)(kernel_file + elf_header->e_phoff);

    // Копируем данные по физическим адресам
    for (unsigned int i = 0; i < elf_header->e_phnum; i++) {
        // Смещение данных в файле
        void* offset = kernel_file + elf_phdr[i].p_offset; 

        memcpy((void*)elf_phdr[i].p_paddr, offset, elf_phdr[i].p_filesz);
    }

    // Точка входа
    void (*entry)() = (void*)elf_header->e_entry;

    entry();

    return;
}