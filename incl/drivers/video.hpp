#include <stdint.h>

#define VGA_FRAMEBUFFER_BASE 0xD0000000

#define VGA_CHAR_HEIGHT 16

namespace nosok {
    namespace video {

        typedef struct {
            int x;
            int y;
        } cursor_pos_t;

        void init(void* framebuffer, unsigned int w, unsigned int h, unsigned int bpp);

        // TODO
        void set_mode(int w, int h, int bpp, int flags);

        // TODO: Функции графического режима
        void put_pixel(cursor_pos_t pos, uint32_t color);
        
        // Функции текстового режима
        void set_cursor_pos(cursor_pos_t pos);
        cursor_pos_t get_cursor_pos();

        void scroll();

        void clear();

        void putc(char c);

    }
}