#ifndef DRIVERS_IO_H
#define DRIVERS_IO_H

#include <stdint.h>

namespace nosok {
    namespace io {
        namespace ports {

            void write8(uint16_t port, uint8_t value);
            void write16(uint16_t port, uint16_t value);
            void write32(uint16_t port, uint32_t value);

            uint8_t read8(uint16_t port);
            uint16_t read16(uint16_t port);
            uint32_t read32(uint16_t port);

        }
    }
}
#endif