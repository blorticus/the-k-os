#include <video/text_terminal.h>
#include <sys/types.h>


term_error term_init( term_entry* te, frame_buffer* fb, u8 font_char_pixel_width, u8 font_char_pixel_height, u8* font_def ) {
    te->fb = fb;
    te->font_char_pixel_width  = font_char_pixel_width;
    te->font_char_pixel_height = font_char_pixel_height;
    te->font_def = font_def;
    te->next_char_x_pos = 0;
    te->next_char_y_pos = 0;
    te->text_cols = (u16)(fb->hpixels) / font_char_pixel_width;
    te->text_rows = (u16)(fb->vpixels) / font_char_pixel_height;
    te->fg_color = 0x00000000;
    te->bg_color = 0x00ffffff;

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
                                     te->fg_color, te->bg_color );

            if ((te->next_char_x_pos = at_x + 1) >= te->text_cols) {
                if (++te->next_char_y_pos == te->text_rows) {
                    // XXX: scroll
                    te->next_char_y_pos = te->text_rows - 1;
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
