#include <stdint.h>

#define PIC_MASTER_CMD		0x20
#define PIC_SLAVE_CMD		0xa0

#define PIC_MASTER_DATA		0x21
#define PIC_SLAVE_DATA		0xa1

namespace nosok {
	namespace cpu {
		namespace interrupts {

			typedef struct {

				uint16_t offset_lo;
				uint16_t segment;
				uint8_t _;

				union {
					struct {
						uint8_t gate 	: 4;
						uint8_t zero 	: 1;
						uint8_t dpl  	: 2;
						bool	present : 1;
					};
					uint8_t flags;
				};

				uint16_t offset_hi;

			}__attribute__((packed)) idt_entry;

			typedef struct {
				uint16_t size;
				idt_entry* linear_addr;
			}__attribute__((packed)) idtr_t;

			void init();

			void mask_irq(int int_n);
			void unmask_irq(int int_n);

			void register_handler(int int_n, void* handler, uint8_t gate_type);
			void register_irq(int int_n, void* handler);

		}
	}
}

