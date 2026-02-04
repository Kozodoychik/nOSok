#include <net/udp.hpp>
#include <net/ip.hpp>
#include <std/string.h>
#include <std/inet.hpp>
#include <std/printf.hpp>
#include <memory/alloc.hpp>

namespace nosok {
	namespace net {
		namespace ip {
			namespace udp {

				UDPSocket* sockets[65536];

				UDPSocket::UDPSocket(uint16_t port) {
					this->port = port;
					memcpy(this->ip, nosok::net::ip::get_ip(), 4);

					sockets[port] = this;

					this->got_packet = false;
				}

				UDPSocket::~UDPSocket() {
					sockets[this->port] = 0;
				}

				void UDPSocket::sendto(uint8_t ip[4], uint16_t port, void* buffer, unsigned int size) {
					udp_header header;

					header.src_port = htons(this->port);
					header.dst_port = htons(port);

					header.size = htons(size + sizeof(udp_header));
					header.checksum = 0;

					send(ip, header, buffer, size);
				}

				void* UDPSocket::recv() {
					
					while (!this->got_packet) {
						asm volatile ("pause");
					}

					//this->got_packet = false;

					void* buffer = this->recv_buffer;

					return buffer;
				}

				void init() {
					memset(sockets, 0, sizeof(sockets));
				}

				void send(uint8_t ip[4], udp_header header, void* buffer, unsigned int size) {
					ipv4_header ip_header;

					memset(&ip_header, 0, sizeof(ip_header));

					memcpy(ip_header.dst_ip, ip, 4);
					ip_header.protocol = 0x11;

					uint8_t* packet = new uint8_t[size + sizeof(udp_header)];

					uint8_t* checksum_buffer = new uint8_t[size + sizeof(udp_header) + sizeof(udp_pseudoheader)];

					udp_pseudoheader* pseudoheader = (udp_pseudoheader*)checksum_buffer;

					memcpy(pseudoheader->dst_ip, ip, 4);
					memcpy(pseudoheader->src_ip, ip::get_ip(), 4);
					pseudoheader->protocol = 0x11;
					pseudoheader->zeros = 0;
					pseudoheader->length = htons(size + sizeof(udp_header));

					memcpy(checksum_buffer + sizeof(udp_pseudoheader), &header, sizeof(udp_header));
					memcpy(checksum_buffer + sizeof(udp_pseudoheader) + sizeof(udp_header), buffer, size);

					uint16_t checksum = compute_checksum(checksum_buffer, size + sizeof(udp_header) + sizeof(udp_pseudoheader));

					delete[] checksum_buffer;

					header.checksum = checksum;

					memcpy(packet, &header, sizeof(udp_header));
					memcpy(packet + sizeof(udp_header), buffer, size);

					nosok::net::ip::send_packet(ip_header, packet, size + sizeof(udp_header));

					delete[] packet;
				}

				void handle_packet(void* buffer) {
					udp_header* header = (udp_header*)buffer;

					if (sockets[ntohs(header->dst_port)] == 0) return;

					sockets[ntohs(header->dst_port)]->recv_buffer = buffer + sizeof(udp_header);
					sockets[ntohs(header->dst_port)]->got_packet = true;
				}

				uint16_t compute_checksum(void* buffer, unsigned int len) {
					uint32_t checksum = 0;
					uint16_t* ptr = (uint16_t*)buffer;

					while (len > 1) {
						checksum += *(ptr++);
						if (checksum & 0x10000)
							checksum = (checksum & 0xffff) + 1;
						len -= 2;
					}

					if (len == 1) {
						checksum += *(uint8_t*)ptr << 8;
						if (checksum & 0x10000)
							checksum = (checksum & 0xffff) + 1;
					}

					return ~((uint16_t)checksum);
				}
			}
		}
	}
}