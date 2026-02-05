#include <net/dhcp.hpp>
#include <net/ethernet.hpp>
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

				void init() {
					socket = new ip::udp::UDPSocket(68);

					uint8_t* discover_msg = new uint8_t[sizeof(dhcp_message)];

					memset(discover_msg, 0, sizeof(dhcp_message));

					dhcp_message* header = (dhcp_message*)discover_msg;

					header->op = 1;
					header->hw_type = 1;
					header->hw_len = 6;
					header->hops = 0;
					header->xid = 0x26262626;
					header->secs = 0;
					header->flags = htons(0x0000);
					
					memcpy(header->client_mac, nosok::net::get_mac(), 6);

					header->dhcp_cookie = DHCP_COOKIE;

					header->vend[0] = DHCP_OPTION_MSG_TYPE;
					header->vend[1] = 1;
					header->vend[2] = 1;

					header->vend[3] = DHCP_OPTION_CLIENT_ID;
					header->vend[4] = 7;
					header->vend[5] = 1;
					memcpy(&header->vend[6], nosok::net::get_mac(), 6);

					header->vend[12] = DHCP_OPTION_PARAMETER_LIST;
					header->vend[13] = 4;
					header->vend[14] = 1;
					header->vend[15] = 3;
					header->vend[16] = 6;
					header->vend[17] = 31;

					header->vend[19] = DHCP_OPTION_END;

					uint8_t broadcast_ip[] = {0xff, 0xff, 0xff, 0xff};

					socket->sendto(broadcast_ip, 67, discover_msg, sizeof(dhcp_message) - (64 - 20));

					void* data = socket->recv();

					nosok::io::printf("RECEIVED!!! %x", ((uint8_t*)data));

				}

			}
		}
	}
}