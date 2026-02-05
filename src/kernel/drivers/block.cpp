#include <drivers/block.hpp>
#include <drivers/pci.hpp>

namespace nosok {
    namespace devices {
        namespace block {

            BlockDevice::BlockDevice() : Device() {
                this->dev_class = DEVICE_BLOCK;
            }

            PCIBlockDevice::PCIBlockDevice(devices::pci::device_info info) : PCIDevice(info), BlockDevice() {};

            void* BlockDevice::read(unsigned int, unsigned int) { return 0; };
            void BlockDevice::write(unsigned int, void*, unsigned int) {};

        }
    }
}