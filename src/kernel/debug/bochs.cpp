#include <debug/bochs.hpp>
#include <drivers/io.hpp>

namespace nosok {
    namespace debug {

        void bochs_print(const char* msg) {
            while (*msg != 0)
                nosok::io::ports::write8(0xe9, *msg++);
        }

    }
}