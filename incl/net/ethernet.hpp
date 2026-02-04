#include <stdint.h>

#define ETH_PROTO_IP	0x0800
#define ETH_PROTO_ARP	0x0806

namespace nosok {
	namespace net {
		typedef struct {
			uint8_t dst_mac[6];
			uint8_t src_mac[6];
			uint16_t ether_type;
		}__attribute__((packed)) ethernet_header_t;

		void init();

		void send(ethernet_header_t header, void* payload, unsigned int size);
		void handle_packet(void* buffer);

		uint8_t* get_mac();
	}
}