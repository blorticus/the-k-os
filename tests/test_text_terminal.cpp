#include <TestSuite.h>
#include <video/SimpleFont.h>
#include <video/fb/cpp-text-terminal.h>
#include <cstdint>

const uint32 TEST_BUF_CHAR_COLS = 8;
const uint32 TEST_BUF_CHAR_ROWS = 8;
const uint32 TEST_BUF_PIXELS_PER_ROW = TEST_BUF_CHAR_COLS * 8;
const uint32 TEST_BUF_LEN = TEST_BUF_CHAR_COLS * 8 * TEST_BUF_CHAR_ROWS * 16;

const uint32 TEST_BUF_FG_COLOR_1 = 0x00123456;
const uint32 TEST_BUF_BG_COLOR_1 = 0x00aabbcc;

const FrameBuffer::Color TEST_BUF_FG_COLOR_2 = FrameBuffer::White;
const FrameBuffer::Color TEST_BUF_BG_COLOR_2 = FrameBuffer::Black;

int main( void ) {
    uint32 buf[TEST_BUF_LEN];

    TestSuite* t = new TestSuite( "FrameBuffer::TextTerminal" );

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_1, (FrameBuffer::Color)TEST_BUF_BG_COLOR_1 );    
    fb->setActiveFont( new Mono8x16SimpleFont() );

    fb->clearScreen();

    t->assertArrayRepeats( buf, TEST_BUF_BG_COLOR_1, (int)TEST_BUF_LEN, "Test 1: clear()" );

    fb->drawCharAt( 79, 0, 0 );

    BufferDescriptor* d = new BufferDescriptor();
    
    d->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16 * (TEST_BUF_CHAR_ROWS - 1), TEST_BUF_BG_COLOR_1 );

    t->assertArraysEqual( buf, d->inflate(), TEST_BUF_LEN, "Test 1: drawCharAt(79,0,0)" );
    
    d->reset();
    
    fb->setColors( TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 );

    fb->clearScreen();

    t->assertArrayRepeats( buf, TEST_BUF_BG_COLOR_2, (int)TEST_BUF_LEN, "Test 2: clear()" );

    fb->drawCharAt( 79, 1, 2 );

    d->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( 16, TEST_BUF_BG_COLOR_2)->addExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->addRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16 * (TEST_BUF_CHAR_ROWS - 2), TEST_BUF_BG_COLOR_2 );

//    t->printTwoValue2dArray( d->inflate(), 64, 8*16, (unsigned int)TEST_BUF_BG_COLOR_2 );
//    t->printTwoValue2dArray( buf, 64, 8*16, (unsigned int)TEST_BUF_BG_COLOR_2 );    

    t->assertArraysEqual( buf, d->inflate(), TEST_BUF_LEN, "Test 2: drawCharAt(79,1,2)" );

//    t->printTwoValue2dArray( d->inflate(), 64, 8*16, (unsigned int)TEST_BUF_BG_COLOR_2 );    
    
    return t->doneTesting();
}
