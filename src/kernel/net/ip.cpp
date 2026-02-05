#include <net/ip.hpp>
#include <net/ethernet.hpp>
#include <net/udp.hpp>
#include <memory/alloc.hpp>
#include <std/string.hpp>
#include <std/inet.hpp>
#include <std/printf.hpp>

namespace nosok {
	namespace net {
		namespace ip {

			uint8_t ip_addr[4] = {0, 0, 0, 0};

			void send_packet(ipv4_header header, void* payload, unsigned int size) {
				ethernet_header_t eth_header;

				eth_header.ether_type = htons(0x0800);

				memcpy(header.src_ip, ip_addr, 4);
				header.id = 0x2626;
				header.header_size = 0x5;
				header.fragment_flags = htons(0x4000);
				header.version = 4;
				header.ttl = 128;
				header.total_length = htons((sizeof(ipv4_header) + size));
				header.header_checksum = htons(compute_checksum(header));

				uint8_t* packet = (uint8_t*)new uint8_t[size + sizeof(ipv4_header)];

				memcpy(packet, &header, sizeof(ipv4_header));
				memcpy(packet + sizeof(ipv4_header), payload, size);

				nosok::net::send(eth_header, packet, size + sizeof(ipv4_header));

				delete[] packet;
			}

			void handle_packet(void* buffer) {
				nosok::io::printf("Got an IP packet!\n");

				ipv4_header* header = (ipv4_header*)buffer;

				nosok::io::printf("Protocol: %x\n", header->protocol);

				switch (header->protocol) {
					case IP_PROTO_UDP: {
						nosok::net::ip::udp::handle_packet(buffer + sizeof(ipv4_header));
						break;
					}
				}
			}

			uint16_t compute_checksum(ipv4_header header) {
				uint32_t sum = 0;
				uint16_t* header_words = (uint16_t*)&header;

				for (unsigned int i = 0; i < (sizeof(ipv4_header) / 2); i++) {
					sum += htons(header_words[i]);
				}

				while ((sum >> 16) != 0)
					sum = (sum & 0xffff) + (sum >> 16);

				return ~(uint16_t)sum;
			}

			uint8_t* get_ip() {
				return ip_addr;
			}

			void set_ip(uint8_t new_ip[6]) {
				memcpy(ip_addr, new_ip, 4);
			}

		}
	}
}