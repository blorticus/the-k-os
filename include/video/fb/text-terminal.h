#ifndef __FB_VIDEO__
#define __FB_VIDEO__

#include <sys/types.h>
#include <sys/unicode.h>

typedef struct {
    unsigned int hrez;
    unsigned int vrez;
    unsigned int columns;
    unsigned int rows;
    unsigned int font_hrez;
    unsigned int font_vrez;
    unsigned int bytes_per_row;
    unsigned int bytes_per_screen;
    void*        fb_first_pixel_addr;
    uint32       fg_color;
    uint32       bg_color;

    unsigned int current_row;
    unsigned int current_col;
} frame_buffer_text_terminal;

typedef frame_buffer_text_terminal* FBTT;

void fbtt_init               ( FBTT fbtt, unsigned int hrez, unsigned int vrez, unsigned int font_hrez_bytes, unsigned int font_vrez_bytes, void* fb_start_addr, uint32 fg_color, uint32 bg_color );
void fbtt_clear_screen       ( FBTT fbtt );
void fbtt_draw_ascii_char_at ( FBTT fbtt, char c, unsigned int row, unsigned int col );
void fbtt_scroll             ( FBTT fbtt, unsigned int rows );

void fbtt_write_char         ( FBTT fbtt, char_t uc );
void fbtt_write_string       ( FBTT fbtt, char_t* s );

#endif
