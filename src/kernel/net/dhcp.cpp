#include <net/dhcp.hpp>
#include <net/ethernet.hpp>
#include <net/ip.hpp>
#include <net/udp.hpp>
#include <std/inet.hpp>
#include <std/printf.hpp>
#include <std/string.hpp>
#include <memory/alloc.hpp>

namespace nosok {
	namespace net {
		namespace ip {
			namespace dhcp {
				
				ip::udp::UDPSocket* socket;

				uint8_t dhcp_server[4];
				uint8_t subnet_mask[4];

				void add_cookie(dhcp_option_builder* b) {
					b->buffer[b->offset++] = 0x63;
					b->buffer[b->offset++] = 0x82;
					b->buffer[b->offset++] = 0x53;
					b->buffer[b->offset++] = 0x63;
				}

				void add_option(dhcp_option_builder* b, uint8_t code, uint8_t length, uint8_t* data) {
					b->buffer[b->offset++] = code;
					b->buffer[b->offset++] = length;

					memcpy(b->buffer + b->offset, data, length);

					b->offset += length;
				}

				void add_client_id(dhcp_option_builder* b, uint8_t hw_type, uint8_t* addr) {
					b->buffer[b->offset++] = 61;
					b->buffer[b->offset++] = 7;
					b->buffer[b->offset++] = hw_type;

					memcpy(b->buffer + b->offset, addr, 6);

					b->offset += 6;
				}

				void add_end(dhcp_option_builder* b) {
					b->buffer[b->offset++] = 0xff;
				}

				dhcp_option read_option(uint8_t* buffer, unsigned int offset) {
					dhcp_option option;

					option.code = buffer[offset++];

					if (option.code == 0xff) return option;

					option.length = buffer[offset++];
					option.data = buffer + offset;

					offset += option.length;

					return option;
				}

				void init() {
					memset(dhcp_server, 4, 0);
					memset(subnet_mask, 4, 0);

					socket = new ip::udp::UDPSocket(68);

					do_discover();
					void* data = socket->recv();

					bootp_message* header = (bootp_message*)data;
					uint8_t* options = (uint8_t*)data + sizeof(bootp_message) + 4;

					if (header->op != 2) return;

					unsigned int offset = 0;

					for (dhcp_option option = read_option(options, offset); option.code != 0xff; option = read_option(options, offset)) {
						
						switch (option.code) {
							case 53: {
								if (option.data[0] != 2) return;
								break;
							}
							case 54: {
								memcpy(dhcp_server, option.data, 4);
								break;
							}

						}

						offset += option.length + 2;
					}

					do_request(header->yiaddr);
					data = socket->recv();

					header = (bootp_message*)data;

					nosok::net::ip::set_ip(header->yiaddr);

					uint8_t* ip = nosok::net::ip::get_ip();

					nosok::io::printf("My IP: %x.%x.%x.%x\n", ip[0], ip[1], ip[2], ip[3]);

				}

				void do_discover() {
					dhcp_option_builder builder = {};
					builder.offset = 0;
					builder.buffer = new uint8_t[256];

					add_cookie(&builder);
					add_option(&builder, 53, 1, (uint8_t[]){0x1});
					add_option(&builder, 55, 3, (uint8_t[]){1, 3, 6});
					add_client_id(&builder, 1, nosok::net::get_mac());
					add_end(&builder);

					int total_length = sizeof(bootp_message) + builder.offset;

					uint8_t* discover_msg = new uint8_t[total_length];

					memset(discover_msg, 0, total_length);

					bootp_message* header = (bootp_message*)discover_msg;

					header->op = 1;
					header->hw_type = 1;
					header->hw_len = 6;
					header->hops = 0;
					header->xid = 0x26262626;
					header->secs = 0;
					header->flags = htons(0x0000);
					
					memcpy(header->client_mac, nosok::net::get_mac(), 6);

					memcpy(discover_msg + sizeof(bootp_message), builder.buffer, builder.offset);

					delete[] builder.buffer;

					uint8_t broadcast_ip[] = {0xff, 0xff, 0xff, 0xff};

					socket->sendto(broadcast_ip, 67, discover_msg, total_length);
				}

				void do_request(uint8_t ip[4]) {
					dhcp_option_builder builder = {};
					builder.offset = 0;
					builder.buffer = new uint8_t[256];

					add_cookie(&builder);
					add_option(&builder, 53, 1, (uint8_t[]){0x3});
					add_option(&builder, 50, 4, ip);
					add_option(&builder, 55, 3, (uint8_t[]){1, 3, 6});
					add_client_id(&builder, 1, nosok::net::get_mac());
					add_end(&builder);

					int total_length = sizeof(bootp_message) + builder.offset;

					uint8_t* discover_msg = new uint8_t[total_length];

					memset(discover_msg, 0, total_length);

					bootp_message* header = (bootp_message*)discover_msg;

					header->op = 1;
					header->hw_type = 1;
					header->hw_len = 6;
					header->hops = 0;
					header->xid = 0x26262626;
					header->secs = 0;
					header->flags = htons(0x0000);
					
					memcpy(header->client_mac, nosok::net::get_mac(), 6);

					memcpy(discover_msg + sizeof(bootp_message), builder.buffer, builder.offset);

					delete[] builder.buffer;

					uint8_t broadcast_ip[] = {0xff, 0xff, 0xff, 0xff};

					socket->sendto(broadcast_ip, 67, discover_msg, total_length);
				}

			}
		}
	}
}