#ifndef __FB_VIDEO__
#define __FB_VIDEO__

#include <sys/types.h>

typedef struct {
    unsigned int hrez;
    unsigned int vrez;
    unsigned int columns;
    unsigned int rows;
    unsigned int font_hrez;
    unsigned int font_vrez;
    void*        fb_first_pixel_addr;
    uint32       fg_color;
    uint32       bg_color;
} frame_buffer_text_terminal;

typedef frame_buffer_text_terminal* FBTT;

void fbtt_init( FBTT fbtt, unsigned int hrez, unsigned int vrez, unsigned int font_hrez_bytes, unsigned int font_vrez_bytes, void* fb_start_addr, uint32 fg_color, uint32 bg_color );

// Divide the text frame buffer into rows and columns, with font_hrez pixels per column and
// font_vrez pixels per row.  Upper left is col=0, row=0
void fbtt_draw_ascii_char_at( FBTT fbtt, char c, unsigned int row, unsigned int col );


#endif
