#pragma once

#include <stdint.h>

namespace nosok {
	namespace net {
		typedef struct {
			uint8_t dst_mac[6];
			uint8_t src_mac[6];
			uint16_t ether_type;
		}__attribute__((packed)) ethernet_header_t;

		void send(ethernet_header_t header, void* payload, unsigned int size);
		void handle_packet(void* buffer);
	}
}