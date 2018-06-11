#pragma once

#include <cstdint>
#include <video/SimpleFont.h>

namespace FrameBuffer {
    enum Color : uint32_t {
        Black           = 0x00000000,
        Blue            = 0x000000ff,
        Green           = 0x0000ff00,
        Red             = 0x00ff0000,
        White           = 0x00ffffff,
    };
    
//    class Font {
//        public:
//            Font();
//            Font( uint8* bitmap, unsigned int char_vrez, unsigned int char_hrez, unsigned chars_defined_in_bitmap );
//            uint8* getBitmapFor( uchar_t codepoint );
//            unsigned int getCharHrez();
//            unsigned int getCharVrez();
//
//        private:
//            uint8*       m_bitmap;
//            unsigned int m_char_vrez;
//            unsigned int m_char_hrez;
//            unsigned int m_chars_defined;
//    };
    
    class TextTerminal {
        public:
            TextTerminal();
            TextTerminal( void* fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color );

            /** Set the address of the first byte of the Frame Buffer into which characters should be written **/
            void setFrameBufferStartAddr(void* fb_start_addr, int hrez, int vrez);
            
            /** Set the font to be used for drawing characters **/
            void setActiveFont(SimpleFont *f);

            /** Change color set **/
            void setColors( Color fg_color, Color bg_color );
            
            /** Clear the text Frame Buffer, setting each pixel to the background color **/
            void clearScreen();
            
            /** Draws a character at the specific location **/
            void drawCharAt( uint32_t c, unsigned int row, unsigned int col );
    
        private:
            uint32_t*    m_fb_first_pixel_addr;
            unsigned int m_hrez;
            unsigned int m_vrez;
            unsigned int m_columns;
            unsigned int m_rows;
            SimpleFont*  m_active_font;
            unsigned int m_font_hrez;
            unsigned int m_font_vrez;
            Color        m_fg_color;
            Color        m_bg_color;
            unsigned int m_current_row;
            unsigned int m_current_col;
            bool         m_initialized;
            unsigned int m_pixels_per_row;
            
            uint32_t* getCharStartPixel( unsigned int char_at_row, unsigned int char_at_col );
    };
}
