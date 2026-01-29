#include <stdint.h>


typedef struct {

    uint16_t width;
    uint16_t height;
    uint8_t  bpp;
    uint8_t* framebuffer;

} display_mode;