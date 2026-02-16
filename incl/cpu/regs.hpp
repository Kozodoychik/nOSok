#include <stdint.h>

typedef struct {
	uint32_t cr2;
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t error_code;
	uint32_t eip;
	uint16_t cs;
	uint32_t eflags;
} regs_t;