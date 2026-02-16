#include <net/udp.hpp>
#include <net/ip.hpp>
#include <std/string.h>
#include <std/inet.hpp>
#include <std/printf.hpp>
#include <std/atomic.hpp>
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
					
					nosok::io::printf("%x\n", this->got_packet);

					while (!this->got_packet.load(__ATOMIC_ACQUIRE)) {
						asm volatile ("hlt");
					}

					this->got_packet.store(false, __ATOMIC_RELEASE);

					this->got_packet = false;

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

					udp_pseudoheader pseudoheader;

					memcpy(pseudoheader.dst_ip, ip, 4);
					memcpy(pseudoheader.src_ip, ip::get_ip(), 4);
					pseudoheader.protocol = 0x11;
					pseudoheader.zeros = 0;
					pseudoheader.length = htons(size + sizeof(udp_header));

					uint16_t checksum = compute_checksum(pseudoheader, header, buffer, size);

					header.checksum = checksum;

					memcpy(packet, &header, sizeof(udp_header));
					memcpy(packet + sizeof(udp_header), buffer, size);

					nosok::net::ip::send_packet(ip_header, packet, size + sizeof(udp_header));

					delete[] packet;
				}

				void handle_packet(void* buffer) {
					udp_header* header = (udp_header*)buffer;

					if (sockets[ntohs(header->dst_port)] == 0) return;

					sockets[ntohs(header->dst_port)]->got_packet.store(true, __ATOMIC_RELEASE);
					sockets[ntohs(header->dst_port)]->recv_buffer = (void*)((uint32_t)buffer + sizeof(udp_header));
				}

				uint16_t compute_checksum(udp_pseudoheader pseudoheader, udp_header header, void* buffer, unsigned int len) {
					uint32_t checksum = 0;
					uint16_t* header_ptr = (uint16_t*)&header;
					uint16_t* pseudoheader_ptr = (uint16_t*)&pseudoheader;
					uint16_t* ptr = (uint16_t*)buffer;

					for (unsigned int i = 0; i < (sizeof(udp_pseudoheader) / 2); i++) {
						checksum += pseudoheader_ptr[i];
						if (checksum & 0x10000)
							checksum = (checksum & 0xffff) + 1;
					}

					for (unsigned int i = 0; i < (sizeof(udp_header) / 2); i++) {
						checksum += header_ptr[i];
						if (checksum & 0x10000)
							checksum = (checksum & 0xffff) + 1;
					}

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