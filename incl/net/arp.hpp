#include <stdint.h>

namespace nosok {
	namespace net {
		namespace arp {

			typedef struct {
				uint16_t	hw_type;
				uint16_t	protocol_type;
				uint8_t		hw_addr_len;
				uint8_t		pr_addr_len;
				uint16_t	opcode;
				uint8_t		src_mac[6];
				uint8_t		src_ip[4];
				uint8_t		dst_mac[6];
				uint8_t		dst_ip[4];
			} arp_packet;
			
		}
	}
}