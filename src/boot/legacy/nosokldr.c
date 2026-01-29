#include <stdint.h>
#include <elf.h>
#include <std/string.h>
#include <boot/display.h>


/*
    kernel_file  - указатель на образ ядра
    disp_mode - информация о видеорежиме
*/
void ldrmain(void* kernel_file, display_mode* disp_mode) {

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