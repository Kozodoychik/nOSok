#include <stdint.h>
#include <memory/frame_alloc.hpp>
#include <memory/paging.hpp>
#include <memory/alloc.hpp>
#include <drivers/video.hpp>
#include <std/printf.hpp>

extern uint32_t stack_end;

extern "C" void kmain() {
    nosok::mem::frames::init();
    nosok::mem::paging::init();
    nosok::mem::heap_allocator_init((void*)&stack_end, 0x100000);


    nosok::io::printf("nOSok Test\nzhe %x", 0x55aa);

    /*nosok::video::putc({'n', 0x07});
    nosok::video::putc({'O', 0x07});
    nosok::video::putc({'S', 0x07});
    nosok::video::putc({'o', 0x07});
    nosok::video::putc({'k', 0x07});

    nosok::video::putc({'\n', 0x07});

    nosok::video::putc({'T', 0x07});
    nosok::video::putc({'e', 0x07});
    nosok::video::putc({'s', 0x07});
    nosok::video::putc({'t', 0x07});*/
    return;
}