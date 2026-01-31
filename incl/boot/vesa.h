#include <stdint.h>

typedef struct {

    uint16_t    attrib;
    uint8_t     win1_attrib;
    uint8_t     win2_attrib;
    uint16_t    win_gran;
    uint16_t    win_size;
    uint16_t    win1_seg;
    uint16_t    win2_seg;
    uint32_t    win_func_ptr;
    uint16_t    bytes_per_scanline;
    uint16_t    width;
    uint16_t    height;
    uint8_t     char_width;
    uint8_t     char_height;
    uint8_t     planes;
    uint8_t     bpp;
    uint8_t     banks;
    uint8_t     mem_model;
    uint8_t     bank_size;
    uint8_t     image_pages;
    uint8_t     _;
    uint8_t     red_mask_size;
    uint8_t     red_field_pos;
    uint8_t     green_mask_size;
    uint8_t     green_field_pos;
    uint8_t     blue_mask_size;
    uint8_t     blue_field_pos;
    uint8_t     res_mask_size;
    uint8_t     res_field_pos;
    uint8_t     direct_color_modeinfo;

    uint32_t    framebuffer;
    uint32_t    offscreen_offs;
    uint16_t    offscreen_size;

    uint8_t     reserved[206];
} vesa_modeinfo;

// Самая адовая структура
typedef struct {
    uint8_t h_freq;
    uint8_t v_freq;
    uint8_t h_active_time;
    uint8_t h_blank_time;
    uint8_t h_active_blank_time;
    uint8_t v_active_time;
    uint8_t v_blank_time;
    uint8_t v_active_blank_time;
    uint8_t hsync_offs;
    uint8_t hsync_pulsewidth;
    uint8_t vsync_offs_pulsewidth;
    uint8_t vhsync_offs_pulsewidth;
    uint8_t h_img_size;
    uint8_t v_img_size;
    uint8_t hv_img_size;
    uint8_t h_border;
    uint8_t v_border;
    uint8_t display_type;
} edid_timing_info;

typedef struct {
    uint64_t          padding;
    uint16_t          manufacture_id;
    uint16_t          edid_id;
    uint32_t          serial;
    uint8_t           manufacture_week;
    uint8_t           manufacture_year;
    uint8_t           version;
    uint8_t           revision;
    uint8_t           video_input_type;
    uint8_t           horizontal_size;
    uint8_t           vertical_size;
    uint8_t           gamma;
    uint8_t           dpms;
    uint8_t           chroma_info[10];
    uint8_t           timings_1;
    uint8_t           timings_2;
    uint8_t           reserved_timings;
    uint16_t          timing_id[8];
    edid_timing_info  timing_descr1;
    edid_timing_info  timing_descr2;
    edid_timing_info  timing_descr3;
    edid_timing_info  timing_descr4;
    uint8_t           _;
    uint8_t           checksum;
} edid_record;


void bios_get_vesa_modeinfo(uint16_t mode, vesa_modeinfo* buffer);
void bios_set_vesa_mode(uint16_t mode);
void bios_get_edid(edid_record* buffer);