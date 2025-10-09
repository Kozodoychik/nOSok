#include <drivers/video.hpp>
#include <drivers/io.hpp>

namespace nosok {
    namespace video {

        vga_character_t* vga_mem = (vga_character_t*)VGA_BASE;

        inline uint32_t _compute_vga_offset(cursor_pos_t pos) {
            return (pos.x + (pos.y * 80));
        }

        cursor_pos_t get_cursor_pos() {
            uint8_t x, y, pos;

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
            
            set_cursor_pos(pos);
        }

    }
}