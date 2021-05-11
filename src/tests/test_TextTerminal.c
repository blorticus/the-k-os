#include <TestSuite.h>
#include <TextTerminal.h>
#include <FrameBuffer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct RuneRetrievalExpected_t {
    Rune rune;
    uint8_t* expectedDefinition;
} *RuneRetrievalExpected;

int main( void ) {
    TestSuite suite = CreateTestSuite( "TextTerminal" );

    TextTerminalFixedFont font = RetrieveTextTerminalFixedFont8x16();

    RuneRetrievalExpected runeExpectMap = (struct RuneRetrievalExpected_t[]){
        (struct RuneRetrievalExpected_t){
            .rune = 0,
            .expectedDefinition = (uint8_t[]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        },
        (struct RuneRetrievalExpected_t){
            .rune = 1024,
            .expectedDefinition = (uint8_t[]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        },
        (struct RuneRetrievalExpected_t){
            .rune = 60,
            .expectedDefinition = (uint8_t[]){ 0x00, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00 },
        },
        (struct RuneRetrievalExpected_t){
            .rune = 255,
            .expectedDefinition = (uint8_t[]){ 0x00, 0x00, 0x6C, 0x6C, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0xF8, 0x00 },
        },
    };

    const uint8_t* glyphDefinition;
    char* nameBuffer = calloc( 100, 1 );

    for (int i = 0; i < 4; i++) {
        glyphDefinition = font->GlyphBitmapForRune( font, runeExpectMap[i].rune );
        sprintf( nameBuffer, "FixedFont8x16 GlyphBitmapForRune( %d )", runeExpectMap[i].rune );

        suite->AssertEquals->ByteArray( suite, runeExpectMap[i].expectedDefinition, glyphDefinition, 16, nameBuffer );
    }

    uint8_t* frameBuffer01_buffer = calloc( 24 * 48 * 4, 1 );
    FrameBuffer frameBuffer01 = calloc( 1, sizeof( struct FrameBuffer_t) );
    TextTerminal term01 = calloc( 1, sizeof(struct TextTerminal_t) );

    PopulateFrameBuffer( frameBuffer01, 24, 48, 32, frameBuffer01_buffer );
    PopulateTextTerminal( term01, frameBuffer01, font );

    Error e = term01->PutRune( term01, 'A' );

    suite->AssertEquals->Int32( suite, NoError, e, "First PutRune to term01 ('A') return value" );

    TestSuitePictureMap pictureMap = CreateTestSuitePictureMap();
    pictureMap->AddCharConversion( pictureMap, ' ', 0 )
              ->AddCharConversion( pictureMap, '*', 0x00ffffff )
              ->Use4ByteEncoding( pictureMap );

    uint8_t* expectedBuffer = pictureMap->ConvertPictureToByteArray( pictureMap,
        "                        "
        "                        "
        "    *                   "
        "   ***                  "
        "  ** **                 "
        " **   **                "
        " **   **                "
        " *******                "
        " **   **                "
        " **   **                "
        " **   **                "
        " **   **                "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        "                        "
        , 24 * 48 );

    suite->AssertEquals->ByteArray( suite, expectedBuffer, frameBuffer01_buffer, 24 * 48 * 4, "First PutRune to term01 ('A') frame buffer" );

    char* pictureToPrint = TestSuiteConvertByteArrayToPicture( frameBuffer01_buffer, 4, 24, 48, (TestSuitePictureMapElement_t[]){
        (TestSuitePictureMapElement_t){ '-', 0x00000000 },
        (TestSuitePictureMapElement_t){ '*', 0x00ffffff },
    }, 2 );

    printf( "%s", pictureToPrint );


    return suite->Done( suite );
}