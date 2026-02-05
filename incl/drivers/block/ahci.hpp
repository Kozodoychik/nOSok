#include <stdint.h>
#include <drivers/block.hpp>
#include <drivers/pci.hpp>

using namespace nosok::devices::pci;

class AHCIBlockDevice : public nosok::devices::block::PCIBlockDevice {
public:

    AHCIBlockDevice(device_info info);

    void init() override;
    void irq() override;

private:
    void* mmio_base;

};

void init();