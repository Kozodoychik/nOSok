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

                Device();
                ~Device();
        };

        void init();
        void register_driver(Device* drv);
    }
}
