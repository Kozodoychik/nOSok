#include <net/ip.hpp>
#include <net/ethernet.hpp>
#include <memory/alloc.hpp>
#include <std/string.hpp>

namespace nosok {
	namespace net {
		namespace ip {

			uint8_t ip_addr[4] = {0, 0, 0, 0};

			void send_packet(ipv4_header header, void* payload, unsigned int size) {
				ethernet_header_t eth_header;

				eth_header.ether_type = 0x08;

				uint8_t* packet = (uint8_t*)new uint8_t[size + sizeof(ipv4_header)];

				memcpy(packet, &header, sizeof(ipv4_header));
				memcpy(packet + sizeof(ipv4_header), payload, size);

				nosok::net::send(eth_header, packet, size + sizeof(ipv4_header));

				delete[] packet;
			}

		}
	}
}