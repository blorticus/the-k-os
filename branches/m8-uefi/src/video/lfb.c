#include <sys/types.h>
#include <video/lfb.h>

lfb_error lfb_init( frame_buffer* fb, u32* buffer_start_addr, u32 hpixels, u32 vpixels, pixel_color_style style ) {
    fb->lfb_base_addr   = buffer_start_addr;
    fb->hpixels         = hpixels;
    fb->vpixels         = vpixels;
    fb->pc_style        = style;

    return LFBE_NoError;
}


lfb_error lfb_draw_boolean_bitmap( frame_buffer* fb, u32 at_x, u32 at_y, u8* bitmap, u32 bm_width, u32 bm_height, u32 bg_color, u32 fg_color ) {
    u32* addr;
    u32 x, y;
    u8 byte;

    addr = fb->lfb_base_addr + (at_y * fb->hpixels) + at_x;

    for (y = 0; y < bm_height; y++) {
        byte = bitmap[y];
        for (x = 0; x < bm_width; x++) {
            if (byte & 0x80)
                *addr++ = fg_color;
            else
                *addr++ = bg_color;

            byte = byte << 1;
        }

        addr += fb->hpixels - bm_width;
    }

    return LFBE_NoError;
}


lfb_error lfb_shift_up( frame_buffer* fb, u32 first_pixel, u32 last_pixel, u32 shift ) {
    
}



lfb_error lfb_fill_background( frame_buffer* fb, u32 bg_color ) {
    u32 lfb_len;
    u32* addr;

    addr = fb->lfb_base_addr;
    lfb_len = fb->hpixels * fb->vpixels;

    while (lfb_len--)
        *addr++ = bg_color;

    return LFBE_NoError;
}
