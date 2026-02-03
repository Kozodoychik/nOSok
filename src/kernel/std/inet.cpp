#include <std/inet.hpp>

uint32_t htonl(uint32_t value) {
	return	((value & 0xff000000) >> 24) | \
			((value & 0x00ff0000) >> 8) | \
			((value & 0x0000ff00) << 8) | \
			((value & 0x000000ff) << 24);
}

uint16_t htonl(uint16_t value) {
	return	((value & 0xff00) >> 8) | \
			((value & 0x00ff) << 8);
}