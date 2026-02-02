#include <cpu/interrupts.hpp>
#include <drivers/io.hpp>

namespace nosok {
	namespace cpu {
		namespace interrupts {

			idt_entry idt[512];

			idtr_t idtr;

			void init() {
				for (int i = 0; i < 512; i++) {
					idt[i].present = false;
					idt[i].zero = 0;
				}

				idtr.size = sizeof(idt);
				idtr.linear_addr = idt;

				uint8_t mask1, mask2;
				mask1 = nosok::io::ports::read8(PIC_MASTER_DATA);
				mask2 = nosok::io::ports::read8(PIC_SLAVE_DATA);

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

				nosok::io::ports::write8(PIC_MASTER_DATA, mask1);
				nosok::io::ports::write8(PIC_SLAVE_DATA, mask2);

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
					nosok::io::ports::write8(PIC_MASTER_DATA, mask2 | (1 << int_n));
			}

			void unmask_irq(int int_n) {
				uint8_t mask1 = nosok::io::ports::read8(PIC_MASTER_DATA);
				uint8_t mask2 = nosok::io::ports::read8(PIC_SLAVE_DATA);
				if (int_n < 8)
					nosok::io::ports::write8(PIC_MASTER_DATA, mask1 & ~(1 << int_n));
				else
					nosok::io::ports::write8(PIC_MASTER_DATA, mask2 & ~(1 << int_n));
			}

			void register_handler(int int_n, void* handler) {

			}

		}
	}
}
