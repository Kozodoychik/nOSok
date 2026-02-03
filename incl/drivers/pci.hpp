#pragma once

#include <stdint.h>
#include <drivers/devices.hpp>

#define PCI_COMMAND 0xcf8
#define PCI_DATA	0xcfc

namespace nosok {
	namespace devices {
		namespace pci {
			typedef struct {
				uint16_t vendor_id;
				uint16_t device_id;

				uint16_t command;
				uint16_t status;

				uint8_t revision_id;
				uint8_t prog_if;
				uint8_t subclass;
				uint8_t class_code;
				uint8_t cache_line_size;
				uint8_t latency_timer;
				uint8_t header_type;
				uint8_t bist;

				uint32_t bar0;
				uint32_t bar1;
				uint32_t bar2;
				uint32_t bar3;
				uint32_t bar4;
				uint32_t bar5;

				uint32_t cis_ptr;
				uint16_t subsystem_vendor_id;
				uint16_t subsystem_id;

				uint32_t exp_rom_base;
				uint8_t capabilities_ptr;

				uint8_t irq_line;
				uint8_t irq_pin;
				uint8_t min_grant;
				uint8_t max_latency;
			} device_header;

			typedef struct {
				uint32_t bus;
				uint32_t device;
				uint32_t function;

				uint16_t vendor_id;
				uint16_t device_id;
			} device_info;

			class PCIDevice : public nosok::devices::Device {
			public:
				device_info info;
				uint16_t io_base;

				PCIDevice(device_info info);
			};

			class PCINetworkDevice : public PCIDevice {
			public:
				PCINetworkDevice(device_info info) : PCIDevice(info) {
					this->dev_class = nosok::devices::NETWORK;
				};

				uint8_t mac[6];

				virtual void send_packet(void* buffer, unsigned int size);
			};

			void init();

			uint32_t get_id(device_info dev, uint32_t reg);

			uint32_t config_read(device_info dev, uint32_t reg);
			uint16_t config_read16(device_info dev, uint32_t reg);

			void config_write(device_info dev, uint32_t reg, uint32_t value);

			device_header get_device_header(device_info dev);

		}
	}
}
