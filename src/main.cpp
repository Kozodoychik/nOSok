#include <stdint.h>
//#include <memory/paging.hpp>
#include <memory/alloc.hpp>
#include <drivers/video.hpp>

extern uint32_t stack_end;

extern "C" void kmain() {
    nosok::mem::heap_allocator_init((void*)&stack_end, 0x100000);

    //nosok::mem::paging::init();
    //nosok::mem::paging::map_page(0xB8000, 0xC03FF00, 1);

    nosok::video::putc({'n', 0x07});
    nosok::video::putc({'O', 0x07});
    nosok::video::putc({'S', 0x07});
    nosok::video::putc({'o', 0x07});
    nosok::video::putc({'k', 0x07});

    nosok::video::putc({'\n', 0x07});

    nosok::video::putc({'T', 0x07});
    nosok::video::putc({'e', 0x07});
    nosok::video::putc({'s', 0x07});
    nosok::video::putc({'t', 0x07});
    return;
}