#include <stdint.h>

#define VGA_BASE 0xC03FF000     // Виртуальный адрес (физический: 0xB8000)

#define VGA_TEXT_MODE 1
#define VGA_GFX_MODE 2

namespace nosok {
    namespace video {

        typedef struct {
            int x;
            int y;
        } cursor_pos_t;

        typedef struct {
            char c;
            uint8_t attr;
        } vga_character_t;

        // TODO
        void set_mode(int w, int h, int bpp, int flags);

        // TODO: Функции графического режима
        void putpixel(int x, int y, uint32_t color);
        
        // Функции текстового режима
        void set_cursor_pos(cursor_pos_t pos);
        cursor_pos_t get_cursor_pos();
        void putc(vga_character_t c);

    }
}