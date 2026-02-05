#include <stdint.h>

namespace nosok {
	namespace net {
		namespace ip {
			namespace udp {

				typedef struct {
					uint16_t	src_port;
					uint16_t	dst_port;
					uint16_t	size;
					uint16_t	checksum;
				}__attribute__((packed)) udp_header;

				typedef struct {
					uint8_t		src_ip[4];
					uint8_t		dst_ip[4];
					uint8_t		zeros;
					uint8_t		protocol;
					uint16_t	length;
				}__attribute__((packed)) udp_pseudoheader;

				class UDPSocket {
				public:
					UDPSocket(uint16_t port);
					~UDPSocket();

					void sendto(uint8_t ip[4], uint16_t port, void* buffer, unsigned int size);
					void* recv();

					uint8_t ip[4];
					uint16_t port;

					void* recv_buffer;
					volatile bool got_packet = false;
				};

				void init();

				void send(uint8_t ip[4], udp_header header, void* buffer, unsigned int size);
				void handle_packet(void* buffer);

				uint16_t compute_checksum(udp_pseudoheader pseudoheader, udp_header header, void* buffer, unsigned int len);

			}
		}
	}
}