#include <std/printf.hpp>
#include <drivers/video.hpp>

namespace nosok {
    namespace io {

        void printf(const char* fmt, ...) {
            uint32_t va_args = (uint32_t)&fmt + sizeof(char*);

            for (const char* c = fmt; *c != 0; c++) {
                switch (*c) {
                    case '%': {
                        switch (*(c+1)) {
                            case 'x': {
                                print_hex(*(uint32_t*)va_args);
                            }
                        }

                        va_args += sizeof(uint32_t);
                        c++;
                        break;
                    }
                    default:
                        nosok::video::putc({*c, 0x07});
                }
            }
        }

        char hex_digits[17] = "0123456789abcdef";

        void print_hex(int val) {
            for (int shift = 28; shift >= 0; shift -= 4) {
                uint8_t nibble = (val >> shift) & 0xf;
                nosok::video::putc({hex_digits[nibble], 0x07});
            }
        }

    }
}