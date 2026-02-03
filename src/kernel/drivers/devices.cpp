#include <drivers/devices.hpp>
#include <std/printf.hpp>

namespace nosok {
	namespace devices {

	Device* driver_registry[1024];

	Device::Device() {

	}

	Device::~Device() {

	}

	void Device::init() {

	}

	void Device::irq() {

	}

	int total_drivers = 0;

		void init() {
			for (int i = 0; i < 1024; i++)
				driver_registry[i] = 0;
		};

		void register_driver(Device* drv) {
			driver_registry[total_drivers] = drv;
			total_drivers++;
		};

		void send_irq(int irq_n) {
			for (int i = 0; i < 1024; i++) {
				if (driver_registry[i]->irq_n == irq_n)
					driver_registry[i]->irq();
			}
		}

	}
}
