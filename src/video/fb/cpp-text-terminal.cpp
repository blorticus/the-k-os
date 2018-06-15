#include <video/fb/cpp-text-terminal.h>
#include <cstdint>
#include <algorithm>

namespace FrameBuffer {
    FrameBuffer::TextTerminal::TextTerminal()
        :   m_fg_color(Black),
            m_bg_color(White),
            m_initialized(false) {
    
    }
    
    FrameBuffer::TextTerminal::TextTerminal(void* fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color)
        :   m_fb_first_pixel_addr((uint32_t*)fb_start_addr),
            m_hrez(hrez),
            m_vrez(vrez),
            m_fg_color(fg_color),
            m_bg_color(bg_color),
            m_current_row(0),
            m_current_col(0),
            m_initialized(false) {
    
    }
    
    void FrameBuffer::TextTerminal::setFrameBufferStartAddr(void* fb_start_addr, int hrez, int vrez) {
        m_fb_first_pixel_addr = (uint32_t*)fb_start_addr;
        m_hrez = hrez;
        m_vrez = vrez;
        
        if (m_active_font)
            m_initialized = true;
    }

    void FrameBuffer::TextTerminal::setActiveFont(SimpleFont *f)
    {
        m_active_font = f;
        m_font_hrez = f->charHrez();
        m_font_vrez = f->charVrez();
        m_columns = m_hrez / m_font_hrez;
        m_rows = m_vrez / m_font_vrez;
        m_current_col = 0;
        m_current_row = 0;
        
        m_pixels_per_row = m_hrez * m_font_vrez;
        
        if (m_fb_first_pixel_addr)
            m_initialized = true;
    }
}

    uint32_t* FrameBuffer::TextTerminal::getCharStartPixel( unsigned int char_at_row, unsigned int char_at_col ) {
        return (uint32_t*)m_fb_first_pixel_addr + (char_at_row * m_pixels_per_row) + (m_font_hrez * char_at_col);
    }
    
    void FrameBuffer::TextTerminal::clearScreen( void ) {
        if (!m_initialized)
            return;
        
        std::fill_n( m_fb_first_pixel_addr, m_hrez * m_vrez, m_bg_color );
    }

    void FrameBuffer::TextTerminal::setColors( Color fg_color, Color bg_color ) {
        m_fg_color = fg_color;
        m_bg_color = bg_color;
    }
    
    void FrameBuffer::TextTerminal::drawCharAt( uint32_t c, unsigned int row, unsigned int col ) {
        if (row > m_rows || col > m_columns)
            return;
        
        uint8_t* char_bitmap = (uint8_t*)m_active_font->bitmapFor( c );
        
        if (!char_bitmap)
            return;
        
        uint32_t* next_pixel = getCharStartPixel( row, col );
        
        for (unsigned int i = 0; i < m_font_vrez; i++) {
            uint8_t nrow = char_bitmap[i];
            for (unsigned int j = 0; j < m_font_hrez; j++) {
                if (nrow & 0x80)
                    *next_pixel++ = (uint32)m_fg_color;
                else
                    *next_pixel++ = (uint32)m_bg_color;
                
                nrow <<= 1;
            }
            
            next_pixel += (m_hrez - m_font_hrez);
        }
    }
    

//void fbtt_scroll( FBTT fbtt, unsigned int rows_to_scroll ) {
//    void* s  = (void*)(fbtt->fb_first_pixel_addr);
//    char* sc = (char*)(fbtt->fb_first_pixel_addr);
//
//    memcpy( s,
//            (const void*)(fbtt->fb_first_pixel_addr + (fbtt->bytes_per_row * rows_to_scroll)),
//            fbtt->bytes_per_screen - (fbtt->bytes_per_row * rows_to_scroll) );
//
//    memset( (void*)(sc + (fbtt->bytes_per_screen - (fbtt->bytes_per_row * rows_to_scroll))),
//            (int)fbtt->bg_color,
//            fbtt->bytes_per_row * rows_to_scroll );
//}
//
//
//void fbtt_write_string( FBTT fbtt, int* s ) {
//    for ( ; *s ; s++ ) {
//        fbtt_write_char( fbtt, *s );
//    }
//}
//
//
//// put character and advance the pointer
//void fbtt_write_char( FBTT fbtt, char_t c ) {
//    // XXX: only handle ASCII at this point
//    if (c < 128) {
//        switch ((char)c) {
//            case '\n':
//                if (++fbtt->current_row >= fbtt->rows) {
//                    fbtt_scroll( fbtt, 1 );
//                    fbtt->current_row = fbtt->rows - 1;
//                }
//                fbtt->current_col = 0;
//                break;
//
//            case '\r':
//                fbtt->current_col = 0;
//                break;
//
//            case '\t':
//                if ((fbtt->current_col += 4) >= fbtt->columns) {
//                    fbtt_write_char( fbtt, (char_t)'\n' );
//                }
//                break;
//
//            case '\b':
//                if (fbtt->current_col == 0) {
//                    if (fbtt->current_row > 0) {
//                        fbtt->current_row--;
//                        fbtt->current_col = fbtt->columns - 1;
//                    }
//                } else {
//                    fbtt->current_col--;
//                }
//                break;
//
//            default:
//                fbtt_draw_ascii_char_at( fbtt, c, fbtt->current_row, fbtt->current_col );
//                if (++fbtt->current_col >= fbtt->columns) {
//                    if (++fbtt->current_row >= fbtt->rows) {
//                        fbtt_scroll( fbtt, 1 );
//                        fbtt->current_row = fbtt->rows - 1;
//                    }
//                    fbtt->current_col = 0;
//                }
//                break;
//        }
//    }
//}
