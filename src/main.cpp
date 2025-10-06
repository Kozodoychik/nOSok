#include <stdint.h>

extern "C" void kmain() {
    *(uint8_t*)0xC03FF000 = 'A';
    *(uint8_t*)0xC03FF001 = 0x06;
    return;
}