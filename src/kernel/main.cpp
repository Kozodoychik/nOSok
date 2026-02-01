#include <stdint.h>
#include <memory/frame_alloc.hpp>
#include <memory/paging.hpp>
#include <memory/alloc.hpp>
#include <drivers/video.hpp>
#include <drivers/pci.hpp>
#include <std/printf.hpp>

extern uint32_t stack_end;

extern "C" void kmain() {
    nosok::mem::frames::init();
    nosok::mem::paging::init();
    nosok::mem::heap_allocator_init((void*)&stack_end, 0x100000);

    nosok::video::clear();

    nosok::video::set_cursor_pos({0, 0});

    nosok::io::printf("nOSok is booting...\n");

    nosok::io::printf("PCI devices:\n");
    
    for (uint16_t bus = 0; bus < 8; bus++) {
        for (uint16_t device = 0; device < 32; device++) {
            uint16_t func = nosok::devices::pci::config_read16({bus, device, 0, 0, 0}, 0x0e) & (1 << 7) ? 8 : 1; // vendor_id и device_id будут заполняться при загрузке драйверов
            for (uint16_t function = 0; function < func; function++) {
                nosok::devices::pci::device_header header = nosok::devices::pci::get_device_header({bus, device, function, 0, 0});
                
                if (header.vendor_id == 0 || header.vendor_id == 0xffff) continue;

                nosok::io::printf(" [%x:%x:%x] vendor_id=%x device_id=%x\n", bus, device, function, header.vendor_id, header.device_id);
            }
        }
    }

    return;
}