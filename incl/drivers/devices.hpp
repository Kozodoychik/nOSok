#pragma once

#include <stdint.h>


namespace nosok {
	namespace devices {
		typedef enum {
			BUS_NONE,
			BUS_PCI,
			BUS_USB
		} bus_type;

		typedef enum {
			DEVICE_BLOCK,
			DEVICE_NETWORK
		} device_class;

		class Device {
		public:
			bus_type bus;
			device_class dev_class;
			uint8_t irq_n = -1;

			Device();
			~Device();

			virtual void irq();
			virtual void init();
		};

		void init();
		void register_driver(Device* drv);

		Device* find_device_by_class(device_class dev_class);

		void send_irq(int irq_n);
	}
}
