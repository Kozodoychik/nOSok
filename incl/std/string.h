#include <stdint.h>


uint8_t* memcpy(uint8_t* dst, uint8_t* src, unsigned int size) {
    for (unsigned int i = 0; i < size; i++)
        dst[i] = src[i];

    return dst;
}