#include <net/ethernet.hpp>
#include <drivers/devices.hpp>
#include <drivers/pci.hpp>
#include <std/printf.hpp>
#include <std/string.hpp>
#include <memory/alloc.hpp>

namespace nosok {
	namespace net {

		uint8_t router_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

		void send(ethernet_header_t header, void* payload, unsigned int size) {
			nosok::devices::pci::PCINetworkDevice* adapter = (nosok::devices::pci::PCINetworkDevice*)nosok::devices::find_device_by_class(nosok::devices::NETWORK);

			uint8_t* frame = new uint8_t[size + sizeof(ethernet_header_t)];

			memcpy(header.src_mac, adapter->mac, 6);
			memcpy(header.dst_mac, router_mac, 6);

			memcpy(frame, &header, sizeof(ethernet_header_t));
			memcpy(frame+sizeof(ethernet_header_t), payload, size);

			adapter->send_packet(frame, size + sizeof(ethernet_header_t));

			delete[] frame;
		}

		void handle_packet(void* buffer) {
			ethernet_header_t* header = (ethernet_header_t*)buffer;

			nosok::io::printf("Got Ethernet packet\nFrom: %2x:%2x:%2x:%2x:%2x:%2x\nTo:   %2x:%2x:%2x:%2x:%2x:%2x",\
				header->src_mac[0], header->src_mac[1], header->src_mac[2], header->src_mac[3], header->src_mac[4], header->src_mac[5], \
				header->dst_mac[0], header->dst_mac[1], header->dst_mac[2], header->dst_mac[3], header->dst_mac[4], header->dst_mac[5]);

		}

	}
}