#include <stdint.h>

#define FRAME_BITMAP_SIZE 0x10000

namespace nosok {
    namespace mem {
        namespace frames {

            void init();

            void* alloc();
            void free(void* addr);

        }
    }
}