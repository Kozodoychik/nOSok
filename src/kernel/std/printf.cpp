#include <std/printf.hpp>
#include <drivers/video.hpp>

namespace nosok {
    namespace io {

        void printf(const char* fmt, ...) {
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

        void print_hex(uint32_t val, int padding) {
            int chars = padding == 0 ? 0 : padding;
            uint32_t tmp = val;

            while (tmp != 0 && padding == 0) {
                tmp >>= 4;
                chars++;
            }

            if (val == 0 && padding == 0) chars = 1;

            for (int c = chars-1; c >= 0; c--) {
                uint8_t nibble = (val >> c*4) & 0xf;
                nosok::video::putc({hex_digits[nibble], 0x07});
            }
        }

    }
}