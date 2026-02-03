#pragma once

#include <stdint.h>


namespace nosok {
	namespace devices {
	typedef enum {
		BUS_NONE,
		BUS_PCI,
		BUS_USB
	} bus_type;

		class Device {
		public:
			bus_type bus;
			uint8_t irq_n;

			Device();
			~Device();

			virtual void irq();
			virtual void init();
		};

		void init();
		void register_driver(Device* drv);

		void send_irq(int irq_n);
	}
}
