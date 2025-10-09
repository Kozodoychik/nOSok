#include <memory/alloc.hpp>

namespace nosok {
    namespace mem {

        heap_block_header_t* heap_top;

        void heap_allocator_init(void* base, uint32_t size) {
            heap_top = (heap_block_header_t*)base;

            heap_top->is_allocated = false;
            heap_top->size = size;
        }

        void* kmalloc(int size) {
            heap_block_header_t* result = 0;

            for (heap_block_header_t* chunk = heap_top; chunk != 0 && result == 0; chunk = chunk->next) {
                if (chunk->size > size && !chunk->is_allocated) result = chunk;
            }
            if (result == 0) return (void*)-1;

            if (result->size > size + sizeof(heap_block_header_t)) {
                heap_block_header_t* tmp = (heap_block_header_t*)((uint32_t)result + sizeof(heap_block_header_t) + size);

                tmp->is_allocated = false;
                tmp->prev = result;
                tmp->next = result->next;
                tmp->size = result->size - size - sizeof(heap_block_header_t);

                if (tmp->next != 0) tmp->next->prev = tmp;

                result->next = tmp;
                result->size = size;
            }

            result->is_allocated = true;

            return (void*)((uint32_t)result + sizeof(heap_block_header_t));

        }

        // TODO
        void kfree(void* ptr) {

        }

    }
}