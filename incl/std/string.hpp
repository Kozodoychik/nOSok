#include <stdint.h>

void* memcpy(void* dst, void* src, unsigned int size);
int memcmp(void* lhs, void* rhs, unsigned int count);
void* memset(void* dst, uint8_t c, unsigned int count);
void* memset16(void* dst, uint16_t c, unsigned int count);