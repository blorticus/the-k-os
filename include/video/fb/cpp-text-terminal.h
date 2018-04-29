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
            TextTerminal( void* fb_start_addr, int hrez, int vrez, uint32 fg_color, uint32 bg_color );

            /** Set the address of the first byte of the Frame Buffer into which characters should be written **/
            void setFrameBufferStartAddr(void* fb_start_addr, int hrez, int vrez);
            
            /** Set the font to be used for drawing characters **/
            void setActiveFont( Font* f );
            
            /** Clear the text Frame Buffer, setting each pixel to the background color **/
            void clear();
    
        private:
            uint32*      m_fb_first_pixel_addr;
            unsigned int m_hrez;
            unsigned int m_vrez;
            unsigned int m_columns;
            unsigned int m_rows;
            Font*        m_active_font;
            unsigned int m_bytes_per_row;
            unsigned int m_bytes_per_screen;
            uint32       m_fg_color;
            uint32       m_bg_color;
            unsigned int m_current_row;
            unsigned int m_current_col;
            bool         m_initialized;
            
            uint32* getCharStartPixel( unsigned int char_at_row, unsigned int char_at_col );
    };
}

#endif
