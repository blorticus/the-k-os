#include <cstdint>
#include <algorithm>
#include <video/fb/TextTerminal.h>
#include <string.h>

FrameBuffer::TextTerminal::TextTerminal()
    : m_fg_color(Black),
      m_bg_color(White),
      m_initialized(false)
{
}

FrameBuffer::TextTerminal::TextTerminal(void *fb_start_addr, int hrez, int vrez, Color fg_color, Color bg_color)
    : m_fb_first_pixel_addr((uint32_t *)fb_start_addr),
      m_hrez(hrez),
      m_vrez(vrez),
      m_fg_color(fg_color),
      m_bg_color(bg_color),
      m_current_row(0),
      m_current_col(0),
      m_initialized(false)
{
}

void FrameBuffer::TextTerminal::setFrameBufferStartAddr(void *fb_start_addr, int hrez, int vrez)
{
    m_fb_first_pixel_addr = (uint32_t *)fb_start_addr;
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

uint32_t *FrameBuffer::TextTerminal::getCharStartPixel(unsigned int char_at_row, unsigned int char_at_col)
{
    return (uint32_t *)m_fb_first_pixel_addr + (char_at_row * m_pixels_per_row) + (m_font_hrez * char_at_col);
}

void FrameBuffer::TextTerminal::clearScreen(void)
{
    if (!m_initialized)
        return;

    std::fill_n((uint32_t*)m_fb_first_pixel_addr, m_hrez * m_vrez, (uint32_t)m_bg_color);
}

void FrameBuffer::TextTerminal::setColors(Color fg_color, Color bg_color)
{
    m_fg_color = fg_color;
    m_bg_color = bg_color;
}

void FrameBuffer::TextTerminal::setCursorPos(unsigned int row, unsigned int col) 
{
    if (row > m_rows || col > m_columns)
        return;

    m_current_row = row;
    m_current_col = col;
}

void FrameBuffer::TextTerminal::drawChar(const wchar_t c) {
    uint8_t *char_bitmap = (uint8_t *)m_active_font->bitmapFor(c);

    switch (c)
    {
        case L'\b':
            if (m_current_col > 0)
                m_current_col--;
            break;

        case L'\t':
            for (int i = 0; i < 4; i++)
                drawChar(L' ');
            break;

        case L'\n':
            m_current_col = 0;
            if (++m_current_row >= m_rows)
                scroll();
            break;

        case L'\r':
            if (++m_current_row >= m_rows) {
                unsigned int c = m_current_col;
                scroll();
                m_current_col = c;
            }
            break;

        default:
            if (!char_bitmap)
            {
                char_bitmap = (uint8_t *)m_active_font->bitmapFor(0);

                if (!char_bitmap)
                    return;
            }

            uint32_t *next_pixel = getCharStartPixel(m_current_row, m_current_col);

            for (unsigned int i = 0; i < m_font_vrez; i++)
            {
                uint8_t nrow = char_bitmap[i];
                for (unsigned int j = 0; j < m_font_hrez; j++)
                {
                    if (nrow & 0x80)
                        *next_pixel++ = (uint32)m_fg_color;
                    else
                        *next_pixel++ = (uint32)m_bg_color;

                    nrow <<= 1;
                }

                next_pixel += (m_hrez - m_font_hrez);
            }

            if (++m_current_col >= m_columns) {
                m_current_col = 0;

                if (++m_current_row >= m_rows) {
                    scroll();
                }
            }

            break;
    }
}

void FrameBuffer::TextTerminal::scroll() {
    void* d = (void*)m_fb_first_pixel_addr;

    // scroll up by one
    memcpy(d, (const void*)(m_fb_first_pixel_addr + m_pixels_per_row), m_pixels_per_row * (m_rows - 1) * 4);
    
    // fill last row
    std::fill_n((uint32_t*)(m_fb_first_pixel_addr + m_pixels_per_row * (m_rows - 1)), m_pixels_per_row, (uint32_t)m_bg_color);
}

void FrameBuffer::TextTerminal::drawString(const wchar_t *str) {
    while (*str)
        drawChar(*str++);
}
