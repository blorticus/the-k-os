#ifndef CPP_TEXT_TERMINAL
#define CPP_TEXT_TERMINAL

#include <sys/types.h>

namespace FrameBuffer {
    class Font {
        public:
            Font();
            Font( uint8* bitmap, unsigned int char_vrez, unsigned int char_hrez, unsigned chars_defined_in_bitmap );
            uint8* getBitmapFor( uint32 codepoint );
            unsigned int getCharHrez();
            unsigned int getCharVrez();

        private:
            uint8*       m_bitmap;
            unsigned int m_char_vrez;
            unsigned int m_char_hrez;
            unsigned int m_chars_defined;
    };
    
    class TextTerminal {
        public:
            TextTerminal();
            TextTerminal( int hrez, int vrez, uint32 fg_color, uint32 bg_color );

            Font* setActiveFont( Font* f );
    
        private:
            unsigned int m_hrez;
            unsigned int m_vrez;
            unsigned int m_columns;
            unsigned int m_rows;
            Font         m_active_font;
            unsigned int m_bytes_per_row;
            unsigned int m_bytes_per_screen;
            void*        m_fb_first_pixel_addr;
            uint32       m_fg_color;
            uint32       m_bg_color;
            unsigned int m_current_row;
            unsigned int m_current_col;
            bool         m_initialized;
    };
}

#endif
