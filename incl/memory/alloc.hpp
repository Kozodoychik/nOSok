#include <stdint.h>

namespace nosok {
    namespace mem {

        typedef struct heap_block_header_t {

            bool is_allocated;
            uint32_t size;
            heap_block_header_t* next;
            heap_block_header_t* prev;

        } heap_block_header_t;

        void heap_allocator_init(void* base, uint32_t size);

        void* kmalloc(int size);

        // TODO
        void kfree(void* ptr);

    }
}