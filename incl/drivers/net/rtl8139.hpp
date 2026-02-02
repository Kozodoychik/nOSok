#include <stdint.h>
#include <drivers/pci.hpp>

using namespace nosok::devices::pci;

class RTL8139: public PCIDevice {
public:
	device_info info = {0, 0, 0, 0x10ec, 0x8139};

	RTL8139();
	~RTL8139();
};
