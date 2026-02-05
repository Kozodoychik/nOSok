#include <stdint.h>

namespace nosok {
	namespace io {

		void printf(const char* fmt, ...);
		void print_hex(uint32_t val, int padding);
		void print_dec(uint32_t val);
	
	}
}