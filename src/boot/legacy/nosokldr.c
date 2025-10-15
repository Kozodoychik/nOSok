#include <stdint.h>

void ldrmain() {

    *(uint32_t*)0xfd000000 = 0xffffffff;

    return;
}