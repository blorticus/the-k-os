#include <TestSuite.h>
#include <video/SimpleFont.h>
#include <video/fb/TextTerminal.h>
#include <cstdint>

const uint32 TEST_BUF_CHAR_COLS = 8;
const uint32 TEST_BUF_CHAR_ROWS = 8;
const uint32 TEST_BUF_PIXELS_PER_ROW = TEST_BUF_CHAR_COLS * 8;
const uint32 TEST_BUF_LEN = TEST_BUF_CHAR_COLS * 8 * TEST_BUF_CHAR_ROWS * 16;

const uint32 TEST_BUF_FG_COLOR_1 = 0x00123456;
const uint32 TEST_BUF_BG_COLOR_1 = 0x00aabbcc;

const FrameBuffer::Color TEST_BUF_FG_COLOR_2 = FrameBuffer::White;
const FrameBuffer::Color TEST_BUF_BG_COLOR_2 = FrameBuffer::Black;

void TestClearScreen(TestSuite* tester) {
    uint32 buf[TEST_BUF_LEN];

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_1, (FrameBuffer::Color)TEST_BUF_BG_COLOR_1 );    
    fb->SetActiveFont( new Mono8x16SimpleFont() );

    fb->ClearScreen();

    tester->AssertArrayRepeats( buf, TEST_BUF_BG_COLOR_1, (int)TEST_BUF_LEN, "TestClearScreen: clear()" );
}

void TestChangeOfColorsBetweenClearScreenCalls(TestSuite* tester) {
    uint32 buf[TEST_BUF_LEN];

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_1, (FrameBuffer::Color)TEST_BUF_BG_COLOR_1 );    

    tester->AssertArrayRepeats( buf, TEST_BUF_BG_COLOR_1, (int)TEST_BUF_LEN,
                                "TestChangeOfColorsBetweenClearScreenCalls: buffer correct after first ClearScreen()" );

    fb->SetActiveFont( new Mono8x16SimpleFont() );
    fb->SetColors( TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 );
    fb->ClearScreen();

    tester->AssertArrayRepeats( buf, TEST_BUF_BG_COLOR_2, (int)TEST_BUF_LEN, 
                               "TestChangeOfColorsBetweenClearScreenCalls: buffer correct after second ClearScreen()" );
}


void TestClearScreenAndDrawSingleRuneAtTopLeft(TestSuite* tester) {
    uint32 buf[TEST_BUF_LEN];

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_2, (FrameBuffer::Color)TEST_BUF_BG_COLOR_2 );
    fb->SetActiveFont( new Mono8x16SimpleFont() );

    fb->ClearScreen();
    fb->DrawRuneAt( 79, 0, 0 );

    BufferDescriptor* d = new BufferDescriptor();
    
    d->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_FG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2, TEST_BUF_BG_COLOR_2 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 8, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_2 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16 * (TEST_BUF_CHAR_ROWS - 1), TEST_BUF_BG_COLOR_2 );

    tester->AssertArraysEqual( buf, d->Inflate(), TEST_BUF_LEN,
                               "TestClearScreenAndDrawSingleChar: buffer matches expected values" );
}

void TestClearScreenAndDrawSingleRuneAtNonCorner(TestSuite* tester) {
    uint32 buf[TEST_BUF_LEN];

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_1, (FrameBuffer::Color)TEST_BUF_BG_COLOR_1 );    
    fb->SetActiveFont( new Mono8x16SimpleFont() );

    fb->ClearScreen();
    fb->DrawRuneAt( 79, 1, 2 );

    BufferDescriptor* d = new BufferDescriptor();
    
    d->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( 16, TEST_BUF_BG_COLOR_1)->AddExplicitValues( 8, new uint32[8] { TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_FG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1, TEST_BUF_BG_COLOR_1 } )->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW - 24, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW, TEST_BUF_BG_COLOR_1 )
     ->AddRepeatingValue( TEST_BUF_PIXELS_PER_ROW * 16 * (TEST_BUF_CHAR_ROWS - 2), TEST_BUF_BG_COLOR_1 );

    tester->AssertArraysEqual( buf, d->Inflate(), TEST_BUF_LEN, "Test 2: drawCharAt(79,1,2)" );
}

void TestWriteLineAtTopLeft(TestSuite* tester) {
    uint32 buf[TEST_BUF_LEN];

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, TEST_BUF_CHAR_COLS * 8, TEST_BUF_CHAR_ROWS * 16, (FrameBuffer::Color)TEST_BUF_FG_COLOR_1, (FrameBuffer::Color)TEST_BUF_BG_COLOR_1 );    
    fb->SetActiveFont( new Mono8x16SimpleFont() );

    fb->ClearScreen();
    fb->WriteLineAt(U"Hello World!", {0, 0});
}

int main( void ) {
    TestSuite* t = new TestSuite( "FrameBuffer::TextTerminal" );

    TestClearScreen( t );
    TestChangeOfColorsBetweenClearScreenCalls( t );
    TestClearScreenAndDrawSingleRuneAtTopLeft( t );
    TestClearScreenAndDrawSingleRuneAtNonCorner( t );
    TestWriteLineAtTopLeft( t );
  
    return t->DoneTesting();
}
