#include <stdint.h>

typedef struct {

    uint16_t    w;
    uint16_t    h;
    uint8_t     bpp;
    void*       fb;

} bootloader_display_info;

typedef struct {
    uint8_t                 boot_drive;
    bootloader_display_info display_info;
} bootloader_info;