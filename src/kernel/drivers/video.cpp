#include <drivers/video.hpp>
#include <drivers/io.hpp>
#include <std/string.h>

namespace nosok {
    namespace video {

        vga_character_t* vga_mem = (vga_character_t*)VGA_BASE;

        uint32_t _compute_vga_offset(cursor_pos_t pos) {
            return pos.y * 80 + pos.x;
        }

        cursor_pos_t get_cursor_pos() {
            uint8_t x, y;
            uint16_t pos;

            nosok::io::ports::write8(0x3D4, 0x0F);
            pos = nosok::io::ports::read8(0x3D5);
            nosok::io::ports::write8(0x3D4, 0x0E);
            pos |= ((uint16_t)nosok::io::ports::read8(0x3D5)) << 8;

            x = pos % 80;
            y = pos / 80;

            return {x, y};
        }

        void set_cursor_pos(cursor_pos_t pos) {
            uint16_t pos_i = _compute_vga_offset(pos);

            nosok::io::ports::write8(0x3D4, 0x0F);
            nosok::io::ports::write8(0x3D5, pos_i & 0xFF); 
            nosok::io::ports::write8(0x3D4, 0x0E);
            nosok::io::ports::write8(0x3D5, pos_i >> 8);
        }

        void scroll() {
            memcpy((void*)VGA_BASE, (void*)(VGA_BASE + 80*2), 80*24*2);
        }

        void clear() {
            memset16((void*)VGA_BASE, 0x0700, 80*25);
        }

        void putc(vga_character_t c) {
            cursor_pos_t pos = get_cursor_pos();
            uint32_t index = _compute_vga_offset(pos);

            switch (c.c) {
                case '\n': {
                    pos.x = 0;
                    pos.y++;
                    break;
                }
                default: {
                    vga_mem[index] = c;
                    pos.x++;
                }

            }

            if (pos.y >= 24) {
                pos.y--;
                scroll();
            }
            
            set_cursor_pos(pos);
        }

    }
}