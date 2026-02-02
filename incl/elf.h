#include <stdint.h>


#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint16_t Elf32_Half;

// Заголовок ELF
typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

// Версия ELF
#define EV_NONE	 0
#define EV_CURRENT  1

// Машина
#define EM_NONE	 0
#define EM_386	  3

// Тип файла
#define ET_NONE	 0
#define ET_EXEC	 2

// Заголовок программы (Program header)
typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

// Тип сегмента в PH
#define PT_NULL	 0
#define PT_LOAD	 1