#include <stdint.h>
#include <drivers/pci.hpp>

using namespace nosok::devices::pci;

class RTL8139: public PCIDevice {
public:

	RTL8139(device_info info);
	~RTL8139();

	void init() override;
	void irq() override;
};
