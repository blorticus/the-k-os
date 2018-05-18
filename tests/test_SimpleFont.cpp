#include <TestSuite.h>
#include <video/SimpleFont.h>
#include <cstdint>

// XXX: at this point, this test suite is very incomplete

int main( void ) {
    TestSuite* t = new TestSuite( "Mono8x16SimpleFont" );

    Mono8x16SimpleFont f = Mono8x16SimpleFont();

    t->assertBool( f.charHrez() == 8, "Font hrez" );
    t->assertBool( f.charVrez() == 16, "Font vrez" );
    t->assertBool( f.hrezEncodingBytes() == 1, "Font hrezEncodingBytes" );

    uint8_t expected[] = { 0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xCE, 0xD6, 0xD6, 0xE6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00 };
    t->assertArraysEqual( (unsigned char*)f.bitmapFor( 48 ), (unsigned char*)expected, 16, "getBitmapFor( 48 )" );

    return t->doneTesting();
}
