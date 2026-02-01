#include <stdint.h>


void* memcpy(void* dst, void* src, unsigned int size) {
    void* old_dst = dst;

    //asm volatile ("rep movsb" : "+D"(dst), "+S"(src), "+c"(size) : : "memory");

    uint8_t* dst8 = (uint8_t*)dst;
    uint8_t* src8 = (uint8_t*)src;

    for (unsigned int i = 0; i < size; i++)
        dst8[i] = src8[i];

    return old_dst;
}

int memcmp(void* lhs, void* rhs, unsigned int count) {
    
    uint8_t* lhs_uint = (uint8_t*)lhs;
    uint8_t* rhs_uint = (uint8_t*)rhs;

    for (unsigned int i = 0; i < count; i++) {
        if (lhs_uint[i] != rhs_uint[i])
            return lhs_uint[i] - rhs_uint[i];
    }

    return 0;
}