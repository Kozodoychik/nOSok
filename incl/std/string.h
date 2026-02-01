#include <stdint.h>


void* memcpy(void* dst, void* src, unsigned int size) {
    void* old_dst = dst;

    asm volatile ("rep movsb" : "+D"(dst), "+S"(src), "+c"(size) : : "memory");

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

void* memset(void* dst, uint8_t c, unsigned int count) {
    void* old_dst = dst;

    asm volatile ("rep stosb" : "=D"(dst), "=c"(count) : "0"(dst), "1"(count), "a"(c) : "memory");

    return old_dst;
}

void* memset16(void* dst, uint16_t c, unsigned int count) {
    void* old_dst = dst;

    asm volatile ("rep stosw" : "=D"(dst), "=c"(count) : "D"(dst), "c"(count), "a"(c) : "memory");

    return old_dst;
}