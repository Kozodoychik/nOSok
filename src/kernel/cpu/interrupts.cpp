#include <cpu/interrupts.hpp>
#include <drivers/io.hpp>
#include <drivers/devices.hpp>
#include <std/printf.hpp>
#include <cpu/regs.hpp>
#include <task/taskmgr.hpp>

extern "C" void isr_2();
extern "C" void isr_6();
extern "C" void isr_13();
extern "C" void isr_14();

extern "C" void irq_32();
extern "C" void irq_33();
extern "C" void irq_34();
extern "C" void irq_35();
extern "C" void irq_36();
extern "C" void irq_37();
extern "C" void irq_38();
extern "C" void irq_39();
extern "C" void irq_40();
extern "C" void irq_41();
extern "C" void irq_42();
extern "C" void irq_43();
extern "C" void irq_44();
extern "C" void irq_45();
extern "C" void irq_46();
extern "C" void irq_47();

namespace nosok {
	namespace cpu {
		namespace interrupts {

			idt_entry idt[256];

			idtr_t idtr;

			void init() {
				for (int i = 0; i < 256; i++) {
					idt[i].present = false;
					idt[i].zero = 0;
					idt[i].segment = 0x55;
				}

				idtr.size = sizeof(idt) - 1;
				idtr.linear_addr = idt;

				nosok::io::ports::write8_slow(PIC_MASTER_CMD, 0x11);
				nosok::io::ports::write8_slow(PIC_SLAVE_CMD, 0x11);

				nosok::io::ports::write8_slow(PIC_MASTER_DATA, 0x20);
				nosok::io::ports::write8_slow(PIC_SLAVE_DATA, 0x28);

				nosok::io::ports::write8_slow(PIC_MASTER_DATA, 0x04);
				nosok::io::ports::write8_slow(PIC_SLAVE_DATA, 0x02);

				nosok::io::ports::write8_slow(PIC_MASTER_DATA, 0x01);
				nosok::io::ports::write8_slow(PIC_SLAVE_DATA, 0x01);

				nosok::io::ports::write8_slow(PIC_MASTER_DATA, 0);
				nosok::io::ports::write8_slow(PIC_SLAVE_DATA, 0);

				nosok::io::ports::write8(PIC_MASTER_DATA, 0xfb);
				nosok::io::ports::write8(PIC_SLAVE_DATA, 0xff);

				register_handler(0x02, (void*)isr_2, 0x0e);
				register_handler(0x06, (void*)isr_6, 0x0e);
				register_handler(0x0d, (void*)isr_13, 0x0e);
				register_handler(0x0e, (void*)isr_14, 0x0e);

				register_handler(0x20, (void*)irq_32, 0x0e);
				register_handler(0x21, (void*)irq_33, 0x0e);
				register_handler(0x22, (void*)irq_34, 0x0e);
				register_handler(0x23, (void*)irq_35, 0x0e);
				register_handler(0x24, (void*)irq_36, 0x0e);
				register_handler(0x25, (void*)irq_37, 0x0e);
				register_handler(0x26, (void*)irq_38, 0x0e);
				register_handler(0x27, (void*)irq_39, 0x0e);
				register_handler(0x28, (void*)irq_40, 0x0e);
				register_handler(0x29, (void*)irq_41, 0x0e);
				register_handler(0x2a, (void*)irq_42, 0x0e);
				register_handler(0x2b, (void*)irq_43, 0x0e);
				register_handler(0x2c, (void*)irq_44, 0x0e);
				register_handler(0x2d, (void*)irq_45, 0x0e);
				register_handler(0x2e, (void*)irq_46, 0x0e);
				register_handler(0x2f, (void*)irq_47, 0x0e);

				asm volatile (
					"lidt %0\nsti" :
					:
					"m"(idtr) :
				);
			}

			void mask_irq(int int_n) {
				uint8_t mask1 = nosok::io::ports::read8(PIC_MASTER_DATA);
				uint8_t mask2 = nosok::io::ports::read8(PIC_SLAVE_DATA);
				if (int_n < 8)
					nosok::io::ports::write8(PIC_MASTER_DATA, mask1 | (1 << int_n));
				else
					nosok::io::ports::write8(PIC_SLAVE_DATA, mask2 | (1 << (int_n-8)));
			}

			void unmask_irq(int int_n) {
				uint8_t mask1 = nosok::io::ports::read8(PIC_MASTER_DATA);
				uint8_t mask2 = nosok::io::ports::read8(PIC_SLAVE_DATA);
				if (int_n < 8)
					nosok::io::ports::write8(PIC_MASTER_DATA, mask1 & ~(1 << int_n));
				else
					nosok::io::ports::write8(PIC_SLAVE_DATA, mask2 & ~(1 << (int_n-8)));
			}

			void register_handler(int int_n, void* handler, uint8_t gate_type) {
				idt[int_n].offset_lo = (uint32_t)handler & 0xffff;
				idt[int_n].offset_hi = (uint32_t)handler >> 16;

				idt[int_n].dpl = 0;
				idt[int_n].segment = 0x8;

				idt[int_n].gate = gate_type;
				idt[int_n].present = true;
			}
		}
	}
}

extern "C" void irq_handler(uint32_t i) {
	nosok::devices::send_irq(i-0x20);
	// EOI
	nosok::io::ports::write8(PIC_MASTER_CMD, 0x20);
	nosok::io::ports::write8(PIC_SLAVE_CMD, 0x20);
}

extern "C" void isr_handler(uint32_t i, regs_t regs) {
	nosok::io::printf("Something went wrong...\n");
	switch (i) {
		case 0x06: {
			nosok::io::printf("EXCEPTION: 0x06 - Invalid opcode\n");
			break;
		}
		case 0x08: {
			nosok::io::printf("EXCEPTION: 0x08 - Double fault\n");
			break;
		}
		case 0x0d: {
			nosok::io::printf("EXCEPTION: 0x0d - GPF (e: %x)\n", regs.error_code);
			break;
		}
		case 0x0e: {
			nosok::io::printf("EXCEPTION: 0x0e - Page fault (e: %x)\n", regs.error_code);
			break;
		}
		default: {
			nosok::io::printf("INTERRUPT: 0x%2x\n", i);
			break;
		}
	}
	nosok::io::printf("\neip=0x%8x cs=0x%4x\neflags=0x%8x\neax=0x%8x ebx=0x%8x ecx=0x%8x edx=0x%8x\nedi=0x%8x esi=0x%8x ebp=0x%8x esp=0x%8x\nds=0x%4x\nes=0x%4x\nfs=0x%4x\ngs=0x%4x\n", \
		regs.eip, regs.cs, regs.eflags, regs.eax, regs.ebx, regs.ecx, regs.edx, regs.edi, regs.esi, regs.ebp, regs.esp, regs.ds, regs.es, regs.fs, regs.gs);

	asm volatile ("cli\nhlt");
}

extern "C" void context_switch_handler() {
	nosok::tasks::switch_task();
}