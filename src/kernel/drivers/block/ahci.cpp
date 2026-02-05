#include <drivers/block/ahci.hpp>
#include <drivers/pci.hpp>
#include <std/printf.hpp>

AHCIBlockDevice::AHCIBlockDevice(device_info info) : nosok::devices::block::PCIBlockDevice(info) {

}

void AHCIBlockDevice::init() {
    nosok::devices::pci::device_header header = nosok::devices::pci::get_device_header(this->info);

    this->mmio_base = (void*)(header.bar5 & 0xfffffffc);

    nosok::io::printf("AHCI MMIO base: %x\n", this->mmio_base);
}

void AHCIBlockDevice::irq() {

}

void init() {

}