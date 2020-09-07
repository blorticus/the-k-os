#include <TestSuite.h>
#include <video/fb/cpp-text-terminal.h>
#include <sys/types.h>

// XXX: at this point, this test suite is very incomplete

int main( void ) {
    TestSuite* t = new TestSuite( "FrameBuffer::Font" );

    FrameBuffer::Font f = FrameBuffer::Font();

    t->assertBool( f.getCharHrez() == 8, "Default Font hrez" );
    t->assertBool( f.getCharVrez() == 16, "Default Font vrez" );

    uint8 expected[] = { 0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xCE, 0xD6, 0xD6, 0xE6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00 };
    t->assertArraysEqual( (unsigned char*)f.getBitmapFor( 48 ), (unsigned char*)expected, 16, "Default Font getBitmapFor( 48 )" );

    return t->doneTesting();
}

