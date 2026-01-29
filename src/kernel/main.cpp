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

    nosok::video::set_cursor_pos({0, 0});
    nosok::io::printf("nOSok Test\n%x", 0x55aa);

    return;
}