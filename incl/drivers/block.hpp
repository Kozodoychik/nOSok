#include <stdint.h>
#include <drivers/devices.hpp>
#include <drivers/pci.hpp>

namespace nosok {
    namespace devices {
        namespace block {
            
            class BlockDevice : public devices::Device {
            public:
                
                BlockDevice();

                virtual void* read(unsigned int lba, unsigned int length);
                virtual void write(unsigned int lba, void* buffer, unsigned int length);

            private:
                uint8_t buffer[512];

            };

            class PCIBlockDevice : public devices::pci::PCIDevice, public BlockDevice {
            public:

                PCIBlockDevice(devices::pci::device_info info);

            };

        }
    }
}