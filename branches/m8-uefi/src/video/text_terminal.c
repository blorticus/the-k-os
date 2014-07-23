#include <video/text_terminal.h>
#include <sys/types.h>
#include <string.h>

term_error term_init( term_entry* te, frame_buffer* fb, u8 font_char_pixel_width, u8 font_char_pixel_height, u8* font_def ) {
    te->fb = fb;
    te->font_char_pixel_width  = font_char_pixel_width;
    te->font_char_pixel_height = font_char_pixel_height;
    te->font_def = font_def;
    te->next_char_x_pos = 0;
    te->next_char_y_pos = 0;
    te->text_cols = (u16)(fb->hpixels) / font_char_pixel_width;
    te->text_rows = (u16)(fb->vpixels) / font_char_pixel_height;
    te->fg_color = 0x00ffffff;
    te->bg_color = 0x00000000;

    return TE_NoError;
}


u16 term_text_columns( term_entry* te ) {
    return te->text_cols;
}


u16 term_text_rows( term_entry* te ) {
    return te->text_rows;
}


term_error term_putchar_at( term_entry* te, char c, u16 at_x, u16 at_y ) {
    u8* fd = te->font_def;

    if (c < 0)
        return TE_InvalidCharacter;

    if (at_x >= te->text_cols || at_y >= te->text_rows)
        return TE_InvalidLocation;

    switch (c) {
        case '\n':
            // XXX
            break;

        case '\b':
            // XXX
            break;

        case '\r':
            // XXX
            break;

        default:
            lfb_draw_boolean_bitmap( te->fb, (u32)(at_x * te->font_char_pixel_width), (u32)(at_y * te->font_char_pixel_height),
                                     (u8*)(fd + ((u8)c * te->font_char_pixel_height)), te->font_char_pixel_width, te->font_char_pixel_height,
                                     te->bg_color, te->fg_color );

            if ((te->next_char_x_pos = at_x + 1) >= te->text_cols) {  // if character places past last column, advance row
                if (++te->next_char_y_pos == te->text_rows) {   // if character position passes last row, scroll and place at start of last row again
                    term_scroll( te, 1 );
                    te->next_char_y_pos = te->text_rows - 1;
                    te->next_char_x_pos = 0;
                }

                te->next_char_x_pos = 0;
            }

            break;
    }

    return TE_NoError;
}


term_error term_putchar( term_entry* te, char c ) {
    return term_putchar_at( te, c, te->next_char_x_pos, te->next_char_y_pos );
}


term_error term_puts_at( term_entry* te, const char* s, u16 at_x, u16 at_y ) {
    char* t = (char*)s;

    te->next_char_x_pos = at_x;
    te->next_char_y_pos = at_y;

    while (*t) 
        term_putchar( te, *t++ );

    return TE_NoError;
}


term_error term_puts( term_entry* te, const char* s ) {
    return term_puts_at( te, s, te->next_char_x_pos, te->next_char_y_pos );
}


term_error term_cls( term_entry* te ) {
    lfb_fill_background( te->fb, te->bg_color );   
    te->next_char_x_pos = 0;
    te->next_char_y_pos = 0;
    return TE_NoError;
}


term_error term_scroll( term_entry* te, u16 rows ) {
    u32 *dest, *src;

    // can't scroll beyond screen height
    if (rows > te->text_rows)
        rows = te->text_rows;


    // shift rows backwards
    dest = te->fb->lfb_base_addr;
    src  = te->fb->lfb_base_addr + rows * te->fb->hpixels * te->font_char_pixel_height;
    memmove( (void*)dest, (const void*)src, (size_t)((te->text_rows - rows) * te->fb->hpixels * te->font_char_pixel_height) );


    // fill in last upshifted rows with spaces (the background color)
    src = te->fb->lfb_base_addr + (te->fb->hpixels * te->fb->vpixels) - (rows * te->fb->hpixels * te->font_char_pixel_height);
    memdwset( (void*)src, te->bg_color, (size_t)(rows * te->fb->hpixels * te->font_char_pixel_height) );

    return TE_NoError;
}
