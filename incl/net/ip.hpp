#include <stdint.h>

#define IP_PROTO_ICMP	0x01
#define IP_PROTO_TCP	0x06
#define IP_PROTO_UDP	0x11

namespace nosok {
	namespace net {
		namespace ip {
			
			typedef struct {
				uint8_t		header_size 	: 4;
				uint8_t		version 		: 4;
				uint8_t 	diff_serv		: 6;
				uint8_t		ecn				: 2;
				uint16_t	total_length;
				uint16_t	id;
				uint16_t	fragment_flags;
				uint8_t		ttl;
				uint8_t		protocol;
				uint16_t	header_checksum;
				uint8_t		src_ip[4];
				uint8_t		dst_ip[4];
			}__attribute__((packed)) ipv4_header;

			void send_packet(ipv4_header header, void* payload, unsigned int size);
			void handle_packet(void* buffer);

			uint16_t compute_checksum(ipv4_header header);

			uint8_t* get_ip();
			void*	set_ip();

		}
	}
}