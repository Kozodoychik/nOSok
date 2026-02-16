#include <std/printf.hpp>
#include <std/string.hpp>
#include <drivers/video.hpp>

namespace nosok {
	namespace io {

		void printf(const char* fmt, ...) {
			asm volatile("cli");
			uint32_t va_args = (uint32_t)&fmt + sizeof(char*);

			for (const char* c = fmt; *c != 0; c++) {
				switch (*c) {
					case '%': {
						int format_param = 0;

						while (*(c+1) >= '0' && *(c+1) <= '9') {
							format_param *= 10;
							format_param += *(c+1) - '0';
							c++;
						}

						switch (*(c+1)) {
							case 'x': {
								print_hex(*(uint32_t*)va_args, format_param);
								break;
							}
							case 'd': {
								print_dec(*(uint32_t*)va_args);
								break;
							}
						}

						va_args += sizeof(uint32_t);
						c++;
						break;
					}
					default:
						nosok::video::putc(*c);
				}
			}
			asm volatile("sti");
		}

		char hex_digits[17] = "0123456789abcdef";

		void print_hex(uint32_t val, int padding) {
			char str[9];
			unsigned int offset = 7;
			uint8_t printed = 0;

			memset(str, 0, 9);

			do {
				uint8_t digit = val & 0xf;
				str[offset--] = hex_digits[digit];
				val >>= 4;
				printed++;
			} while (val != 0);

			for (int i = 0; i < padding - printed; i++)
				str[offset--] = '0';

			printf(str + offset+1);
		}

		void print_dec(uint32_t val) {
			char str[11];
			unsigned int offset = 9;
			memset(str, 0, 11);

			do {
				uint8_t digit = val % 10;
				str[offset--] = digit + '0';
				val /= 10;
			} while (val != 0);

			printf(str + offset+1);
		}

	}
}