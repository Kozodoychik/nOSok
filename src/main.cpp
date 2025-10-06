#include <stdint.h>
#include <drivers/video.h>

extern "C" void kmain() {
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