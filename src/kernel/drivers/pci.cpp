#include <drivers/pci.hpp>
#include <drivers/devices.hpp>
#include <drivers/io.hpp>
#include <memory/alloc.hpp>
#include <std/printf.hpp>

#include <drivers/block/ahci.hpp>
#include <drivers/net/rtl8139.hpp>
#include <drivers/net/pcnet.hpp>

namespace nosok {
	namespace devices {
		namespace pci {

			PCIDevice::PCIDevice(device_info info) : Device() {
				this->bus = BUS_PCI;

				this->info.bus = info.bus;
				this->info.device = info.device;
				this->info.function = info.function;
			}

			/*PCINetworkDevice::PCINetworkDevice(device_info info) : PCIDevice(info) {
				this->dev_class = nosok::devices::NETWORK;
			}*/

			void init() {
				for (uint16_t bus = 0; bus < 8; bus++){
					for (uint16_t device = 0; device < 32; device++) {
						uint16_t func = config_read16({bus, device, 0, 0, 0}, 0x0e) & (1 << 7) ? 8 : 1;
						for (uint16_t function = 0; function < func; function++) {
							device_header header = get_device_header({bus, device, function, 0, 0});
						
							switch (header.vendor_id) {
								case 0x10ec: { // Realtek
									switch (header.device_id) {
										case 0x8139: { // RTL8139
											/*PCIDevice* driver = new RTL8139({bus, device, function, 0, 0});
											nosok::devices::register_driver(driver);*/
											break;
										}
									}
									break;
								}
								case 0x1022: { // AMD
									switch (header.device_id) {
										case 0x2000: { // AMD PCnet-PCI II
											PCIDevice* driver = new PCNET({bus, device, function, 0, 0});
											register_driver(driver);
											driver->init();
											break;
										}
									}
									break;
								}
							}

							switch (header.class_code) {
								case 0x01: { // Mass storage
									switch (header.subclass) {
										case 0x06: { // SATA
											switch (header.prog_if) {
												case 0x01: { // AHCI
													PCIDevice* driver = new AHCIBlockDevice({bus, device, function, 0, 0});
													register_driver(driver);
													driver->init();
													break;
												}
											}
											break;
										}
									}
									break;
								}
							}
						}
					}
				}
			}

			uint32_t get_id(device_info dev, uint32_t reg){
				return (1 << 31) | ((dev.bus & 0xff) << 16) | ((dev.device & 0x1f) << 11) | ((dev.function & 0x07) << 8) | (reg & 0xfc);
			}

			uint32_t config_read(device_info dev, uint32_t reg) {
				uint32_t id = get_id(dev, reg);

				nosok::io::ports::write32(PCI_COMMAND, id);
				uint32_t config = nosok::io::ports::read32(PCI_DATA);

				return config;
			}

			uint16_t config_read16(device_info dev, uint32_t reg) {
				uint32_t id = get_id(dev, reg);

				nosok::io::ports::write32(PCI_COMMAND, id);
				uint32_t config = nosok::io::ports::read32(PCI_DATA);

				return (uint16_t)(config >> ((reg & 2) * 8) & 0xffff);
			}

			void config_write(device_info dev, uint32_t reg, uint32_t value) {
				uint32_t id = get_id(dev, reg);

				nosok::io::ports::write32(PCI_COMMAND, id);
				nosok::io::ports::write32(PCI_DATA, value);
			}

			device_header get_device_header(device_info dev) {
				device_header header;

				header.vendor_id = config_read16(dev, 0x00);
				header.device_id = config_read16(dev, 0x02);

				header.command = config_read16(dev, 0x04);
				header.status = config_read16(dev, 0x06);

				header.revision_id = (uint8_t)config_read16(dev, 0x08) & 0xff;
				header.prog_if = (uint8_t)(config_read16(dev, 0x08) >> 8);

				header.subclass = (uint8_t)config_read16(dev, 0x0a) & 0xff;
				header.class_code = (uint8_t)(config_read16(dev, 0x0a) >> 8);

				header.cache_line_size = (uint8_t)config_read16(dev, 0x0c) & 0xff;
				header.latency_timer = (uint8_t)(config_read16(dev, 0x0c) >> 8);

				header.header_type = (uint8_t)config_read16(dev, 0x0e) & 0xff;
				header.bist = (uint8_t)(config_read16(dev, 0x0e) >> 8);

				header.bar0 = config_read(dev, 0x10);
				header.bar1 = config_read(dev, 0x14);
				header.bar2 = config_read(dev, 0x18);
				header.bar3 = config_read(dev, 0x1c);
				header.bar4 = config_read(dev, 0x20);
				header.bar5 = config_read(dev, 0x24);

				header.cis_ptr = config_read(dev, 0x28);
				header.subsystem_vendor_id = config_read16(dev, 0x2c);
				header.subsystem_id = config_read16(dev, 0x2e);

				header.exp_rom_base = config_read(dev, 0x30);

				header.capabilities_ptr = (uint8_t)(config_read16(dev, 0x34) & 0xff);

				header.irq_line = (uint8_t)(config_read16(dev, 0x3c) & 0xff);
				header.irq_pin = (uint8_t)(config_read16(dev, 0x3c) >> 8);

				header.min_grant = (uint8_t)(config_read16(dev, 0x3e) & 0xff);
				header.max_latency = (uint8_t)(config_read16(dev, 0x3e) >> 8);

				return header;
			}

		}
	}
}
