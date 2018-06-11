#include <video/fb/TextTerminal.h>
#include <cstdint>
#include <algorithm>

namespace FrameBuffer
{
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
} // namespace FrameBuffer

uint32_t *FrameBuffer::TextTerminal::getCharStartPixel(unsigned int char_at_row, unsigned int char_at_col)
{
    return (uint32_t *)m_fb_first_pixel_addr + (char_at_row * m_pixels_per_row) + (m_font_hrez * char_at_col);
}

void FrameBuffer::TextTerminal::clearScreen(void)
{
    if (!m_initialized)
        return;

    std::fill_n(m_fb_first_pixel_addr, m_hrez * m_vrez, m_bg_color);
}

void FrameBuffer::TextTerminal::setColors(Color fg_color, Color bg_color)
{
    m_fg_color = fg_color;
    m_bg_color = bg_color;
}

void FrameBuffer::TextTerminal::drawCharAt(uint32_t c, unsigned int row, unsigned int col)
{
    if (row > m_rows || col > m_columns)
        return;

    uint8_t *char_bitmap = (uint8_t *)m_active_font->bitmapFor(c);

    if (!char_bitmap)
        return;

    uint32_t *next_pixel = getCharStartPixel(row, col);

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
}