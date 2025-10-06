#include <drivers/io.hpp>

namespace nosok {
    namespace io {
        namespace ports {
            void write8(uint16_t port, uint8_t value) {
                asm (
                    "outb %0, %1":
                    :
                    "a"(value),"Nd"(port):
                    "memory"
                );
            }
            void write16(uint16_t port, uint16_t value) {
                asm (
                    "outw %0, %1":
                    :
                    "a"(value),"Nd"(port):
                    "memory"
                );
            }
            void write32(uint16_t port, uint32_t value) {
                asm (
                    "outl %0, %1":
                    :
                    "a"(value),"Nd"(port):
                    "memory"
                );
            }

            uint8_t read8(uint16_t port) {
                uint8_t data;

                asm (
                    "inb %1, %0":
                    "=a"(data):
                    "Nd"(port):
                    "memory"
                );

                return data;
            }
            uint16_t read16(uint16_t port) {
                uint16_t data;
                
                asm (
                    "inw %1, %0":
                    "=a"(data):
                    "Nd"(port):
                    "memory"
                );

                return data;
            }
            uint32_t read32(uint16_t port) {
                uint32_t data;
                
                asm (
                    "inl %1, %0":
                    "=a"(data):
                    "Nd"(port):
                    "memory"
                );

                return data;
            }
        }
    }
}