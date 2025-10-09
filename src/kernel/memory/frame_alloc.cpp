#include <memory/frame_alloc.hpp>

extern uint32_t _kernel_end;

namespace nosok {
    namespace mem {
        namespace frames {

            uint8_t bitmap[FRAME_BITMAP_SIZE];
            uint32_t base;

            void _mark_as_owned(int i) {
                int bitmap_byte = i / 8;
                int byte_index = i % 8;

                bitmap[bitmap_byte] = bitmap[bitmap_byte] | (1 << byte_index);
            }

            void _mark_as_free(int i) {
                int bitmap_byte = i / 8;
                int byte_index = i % 8;

                bitmap[bitmap_byte] = bitmap[bitmap_byte] & ~(1 << byte_index);
            }

            bool _get(int i) {
                int bitmap_byte = i / 8;
                int byte_index = i % 8;

                return bitmap[bitmap_byte] & (1 << byte_index);
            }

            void init() {
                for (int i = 0; i*4096 < (unsigned int)&_kernel_end + 0x10000; i++) {
                    _mark_as_owned(i);
                }
            }

            void* alloc() {
                for (int i = 0; i < 8 * FRAME_BITMAP_SIZE; i++) {
                    if (!_get(i)) {
                        _mark_as_owned(i);
                        return (void*)(i * 4096);
                    }
                }

                return (void*)-1;
            }

            void free(void* ptr) {
                unsigned int i = (unsigned int)ptr / 4096;

                _mark_as_free(i);
            }

        }
    }
}