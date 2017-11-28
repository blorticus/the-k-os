#include <video/fb/text-terminal.h>
#include <video/font.h>
#include <string.h>

void fbtt_init( FBTT fbtt, unsigned int hrez, unsigned int vrez, unsigned int font_hrez_bytes, unsigned int font_vrez_bytes, void* fb_start_addr, uint32 fg_color, uint32 bg_color ) {
    fbtt->hrez             = hrez;
    fbtt->vrez             = vrez;
    fbtt->font_hrez        = 8 * font_hrez_bytes;
    fbtt->font_vrez        = 8 * font_vrez_bytes;
    fbtt->columns          = hrez / fbtt->font_hrez;
    fbtt->rows             = vrez / fbtt->font_vrez;

    fbtt->bytes_per_row    = hrez * fbtt->font_vrez * 4;
    fbtt->bytes_per_screen = fbtt->bytes_per_row * fbtt->rows;

    fbtt->fb_first_pixel_addr = fb_start_addr;

    fbtt->fg_color = fg_color;
    fbtt->bg_color = bg_color;
}

static __attribute__((always_inline)) inline unsigned int* _get_start_pixel( FBTT fbtt, unsigned int row, unsigned int column ) {
    return fbtt->fb_first_pixel_addr + (row * fbtt->font_vrez + column * fbtt->font_hrez);
}

// Divide the text frame buffer into rows and columns, with font_hrez pixels per column and
// font_vrez pixels per row.  Upper left is col=0, row=0
void fbtt_draw_ascii_char_at( FBTT fbtt, char c, unsigned int row, unsigned int col ) {
    uint32 *fbb = (uint32*)(fbtt->fb_first_pixel_addr);
    int i, j;
    const uint32 bg = 0x00000000;
    const uint32 fg = 0xffffffff;
    uint8 col_def;
    uint8* fontmap;

    fbb += (row * fbtt->hrez * fbtt->font_vrez + col * fbtt->font_hrez);

    fontmap = (uint8*)(font_get_definition( ASCII_8x16 ));

    for (i = 0; i < fbtt->font_vrez; i++) {
        col_def = *(fontmap + (c * fbtt->font_vrez + i));

        for (j = 0; j < fbtt->font_hrez; j++, col_def <<= 1) {
            // XXX: This would only work for font_hrez = 8; need to adjust for varying sizes
            if (col_def & 0x80)
                *fbb++ = fg;
            else
                *fbb++ = bg;
        }

        fbb += (fbtt->hrez - fbtt->font_hrez);
    }
}

void fbtt_clear_screen( FBTT fbtt ) {
    void* s = (void*)(fbtt->fb_first_pixel_addr);
    memset( s, (int)fbtt->bg_color, fbtt->hrez * fbtt->vrez * 4 );
}

void fbtt_scroll( FBTT fbtt, unsigned int rows_to_scroll ) {
    void* s  = (void*)(fbtt->fb_first_pixel_addr);
    char* sc = (char*)(fbtt->fb_first_pixel_addr);

    memcpy( s,
            (const void*)(fbtt->fb_first_pixel_addr + (fbtt->bytes_per_row * rows_to_scroll)),
            fbtt->bytes_per_screen - (fbtt->bytes_per_row * rows_to_scroll) );

    memset( (void*)(sc + (fbtt->bytes_per_screen - (fbtt->bytes_per_row * rows_to_scroll))),
            (int)fbtt->bg_color,
            fbtt->bytes_per_row * rows_to_scroll );
}
