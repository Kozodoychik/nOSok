#include <stdint.h>
#include <memory/frame_alloc.hpp>
#include <memory/paging.hpp>
#include <memory/alloc.hpp>
#include <drivers/video.hpp>
#include <drivers/pci.hpp>
#include <drivers/io.hpp>
#include <std/printf.hpp>
#include <std/string.hpp>
#include <std/inet.hpp>
#include <boot/bootinfo.h>
#include <cpu/interrupts.hpp>
#include <net/ethernet.hpp>
#include <net/arp.hpp>
#include <net/udp.hpp>
#include <net/dhcp.hpp>
#include <task/taskmgr.hpp>
#include <debug/bochs.hpp>


extern uint32_t stack_end;


void test() {
	while(1) {
		nosok::io::printf("Hello from thread No2\n");
	}
}

void test2() {
	while(1) {
		nosok::io::printf("Hello from thread No3\n");
	}
}

extern "C" void kmain(bootloader_info* boot_info) {
	nosok::mem::frames::init();
	nosok::mem::paging::init();
	nosok::mem::heap_allocator_init((void*)&stack_end, 0xffffffff);
	nosok::debug::bochs_print("nOSok is booting...\n");

	nosok::video::init(boot_info->display_info.fb, boot_info->display_info.w, boot_info->display_info.h, boot_info->display_info.bpp, boot_info->display_info.pitch);
	nosok::video::clear();
	nosok::video::set_cursor_pos({0, 0});

	nosok::cpu::interrupts::init();

	nosok::io::printf("nOSok is booting...\n");
	nosok::io::printf("framebuffer base: 0x%8x\n", boot_info->display_info.fb);

	nosok::devices::pci::init();

	nosok::io::printf("w: 0x%x, h: 0x%x, bpp 0x%x\n", boot_info->display_info.w, boot_info->display_info.h, boot_info->display_info.bpp);

	nosok::io::printf("PCI devices:\n");
	
	for (uint16_t bus = 0; bus < 8; bus++) {
		for (uint16_t device = 0; device < 32; device++) {
			uint16_t func = nosok::devices::pci::config_read16({bus, device, 0, 0, 0}, 0x0e) & (1 << 7) ? 8 : 1; // vendor_id и device_id будут заполняться при загрузке драйверов
			for (uint16_t function = 0; function < func; function++) {
				nosok::devices::pci::device_header header = nosok::devices::pci::get_device_header({bus, device, function, 0, 0});
				
				if (header.vendor_id == 0 || header.vendor_id == 0xffff) continue;

				nosok::io::printf(" [0x%1x:0x%1x:0x%1x] vendor_id=%x device_id=%x\n", bus, device, function, header.vendor_id, header.device_id);
			}
		}
	}

	nosok::net::init();
	nosok::net::arp::init();
	nosok::net::ip::udp::init();
	nosok::net::ip::dhcp::init();

	nosok::tasks::init();
	nosok::cpu::interrupts::unmask_irq(0);

	nosok::tasks::create((void*)test);
	nosok::tasks::create((void*)test2);

	while(1) {
		nosok::io::printf("Hello from thread No1\n");
	}

	return;
}
