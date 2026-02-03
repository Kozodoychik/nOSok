#include <drivers/video.hpp>
#include <boot/vesa.h>
#include <drivers/io.hpp>
#include <memory/paging.hpp>
#include <std/string.h>

extern uint8_t display_font[4096];

namespace nosok {
	namespace video {

		uint8_t* vga_mem = (uint8_t*)VGA_FRAMEBUFFER_BASE;
		cursor_pos_t cursor_pos;

		unsigned int scr_w, scr_h, scr_bpp, scr_pitch;

		uint32_t _compute_vga_offset(cursor_pos_t pos) {
			return (pos.y * scr_pitch + (pos.x * (scr_bpp / 8)));
		}

		void init(void* framebuffer, unsigned int w, unsigned int h, unsigned int bpp, unsigned int pitch) {
			int pages_for_framebuffer = (pitch * h * (bpp / 8)) / 4096;

			for (int i = 0; i <= pages_for_framebuffer; i++) {
				uint32_t offset = i * 4096;
				nosok::mem::paging::map_page(offset + (uint32_t)framebuffer, offset + VGA_FRAMEBUFFER_BASE);
			}

			cursor_pos = {0, 0};
			
			scr_w = w;
			scr_h = h;
			scr_bpp = bpp;
			scr_pitch = pitch;
		}

		void put_pixel(unsigned int x, unsigned int y, uint32_t color) {
			int offset = _compute_vga_offset({x, y});
			vga_mem[offset] = color & 0xff;
			vga_mem[offset + 1] = (color >> 8) & 0xff;
			vga_mem[offset + 2] = (color >> 16) & 0xff;
		}

		cursor_pos_t get_cursor_pos() {
			  return cursor_pos;
		}

		void set_cursor_pos(cursor_pos_t pos) {
			cursor_pos = pos;
		}

		void scroll() {
			memcpy((void*)VGA_FRAMEBUFFER_BASE, (void*)(VGA_FRAMEBUFFER_BASE + scr_pitch*VGA_CHAR_HEIGHT), scr_pitch*(scr_h-VGA_CHAR_HEIGHT));
		}

		void clear() {
			memset((void*)VGA_FRAMEBUFFER_BASE, 0x00, scr_pitch*scr_h*(scr_bpp/8));
		}

		void putc(char c) {
			cursor_pos_t pos = get_cursor_pos();

			switch (c) {
				case '\n': {
					pos.x = 0;
					pos.y++;
					break;
				}
				default: {
					unsigned int glyph = (unsigned int)c * VGA_CHAR_HEIGHT;
					cursor_pos_t p = {pos.x * 8, pos.y * VGA_CHAR_HEIGHT};

					for (int y = 0; y < VGA_CHAR_HEIGHT; y++) {
						uint8_t line = display_font[glyph + y];
						for (int x = 0; x < 8; x++) {
							uint8_t pixel = (line << x) & 0x80;
							put_pixel(p.x + x, p.y + y, pixel ? 0xffffff : 0);
						}
					}
					pos.x++;
				}

			}

			if (pos.x >= scr_w / 8) {
				pos.x = 0;
				pos.y++;
			}

			if (pos.y >= (scr_h / VGA_CHAR_HEIGHT) - 1) {
				pos.y--;
				scroll();
			}

			set_cursor_pos(pos);
		}

	}
}