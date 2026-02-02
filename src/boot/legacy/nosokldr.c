#include <stdint.h>
#include <elf.h>
#include <std/string.h>
#include <boot/vesa.h>
#include <boot/drive.h>
#include <boot/bootinfo.h>
#include <fs/fat.h>

#define FAT_ADDR			0x8000
#define KERNEL_IMAGE_ADDR   0x40000

extern uint8_t boot_drive;

vesa_modeinfo setup_display() {
	vesa_modeinfo mode;
	edid_record edid;

	bios_get_vesa_modeinfo(0x4115, &mode);

	bios_get_edid(&edid);

	if (edid.padding != 0x00ffffffffffff00) {
		bios_set_vesa_mode(0x115);
		return mode;
	}

	uint16_t width = edid.timing_descr1.h_active_time | ((edid.timing_descr1.h_active_blank_time & 0xf0) << 4);
	uint16_t height = edid.timing_descr1.v_active_time | ((edid.timing_descr1.v_active_blank_time & 0xf0) << 4);

	for (uint16_t m = 0x4100; m < 0xffff; m++) {
		bios_get_vesa_modeinfo(m, &mode);

		if (mode.width == width && mode.height == height && (mode.bpp == 24 || mode.bpp == 32) && mode.attrib & 0x90 && mode.mem_model == 6) {
			bios_set_vesa_mode(m);
			return mode;
		}
	}

	bios_set_vesa_mode(0x4115);

	return mode;
}

void ldrmain() {
	void* kernel_file = (void*)KERNEL_IMAGE_ADDR;

	// Получаем BIOS Parameter Block (уже загружен)
	fat_bpb* bpb = (fat_bpb*)0x7c00;

	// Считаем первые сектора корневого каталога и данных
	uint16_t root_sect = bpb->reserved_count + (bpb->fat_count * bpb->secs_per_fat);
	uint16_t first_data_sect = root_sect + ((bpb->root_dir_entries * 32) / bpb->bytes_per_sector);

	fat_directory_entry root_directory[512];

	// Читаем FAT и корневой каталог
	bios_drive_read(bpb->reserved_count, bpb->secs_per_fat, (void*)FAT_ADDR);
	bios_drive_read(root_sect, 1, &root_directory);

	// Ищем образ
	for (uint32_t i = 0; i < bpb->root_dir_entries; i++) {
		if (memcmp(root_directory[i].filename, "KERNEL     ", 11) == 0) {
			uint16_t cluster = root_directory[i].first_cluster_lo;
			uint32_t offset = 0;

			// Читаем образ
			while (cluster < 0xfff8) {
				bios_drive_read(first_data_sect + ((cluster-2) * bpb->secs_per_cluster), bpb->secs_per_cluster, (kernel_file + offset));
				cluster = *(((uint16_t*)FAT_ADDR) + cluster);
				offset += bpb->secs_per_cluster * bpb->bytes_per_sector;
			}
		}
	}

	// Получаем заголовок в самом начале файла
	Elf32_Ehdr* elf_header = (Elf32_Ehdr*)kernel_file;

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
	void (*entry)(bootloader_info*) = (void*)elf_header->e_entry;

	vesa_modeinfo mode = setup_display();
	bootloader_info boot_info;

	boot_info.boot_drive = boot_drive;
	
	boot_info.display_info.w = mode.width;
	boot_info.display_info.h = mode.height;
	boot_info.display_info.bpp = mode.bpp;
	boot_info.display_info.pitch = mode.pitch;
	boot_info.display_info.fb = (void*)mode.framebuffer;

	entry(&boot_info);

	return;
}