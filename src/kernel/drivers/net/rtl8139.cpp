#include <drivers/net/rtl8139.hpp>
#include <drivers/pci.hpp>
#include <std/printf.hpp>

RTL8139::RTL8139(device_info info) : PCIDevice(info) {
	nosok::io::printf("RTL8139 is trying to initialize, but driver is not implemented lol\n");
}

void RTL8139::init() {
	
}

void RTL8139::irq() {

}