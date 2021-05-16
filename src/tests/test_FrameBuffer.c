#include <TestSuite.h>
#include <FrameBuffer.h>
#include <Error.h>
#include <string.h>
#include <stdio.h>

#define FRAME_BUFFER_MEMORY_AREA_HREZ 12
#define FRAME_BUFFER_MEMORY_AREA_VREZ 10
#define FRAME_BUFFER_PIXEL_COUNT (FRAME_BUFFER_MEMORY_AREA_HREZ * FRAME_BUFFER_MEMORY_AREA_VREZ)
#define FRAME_BUFFER_BYTE_COUNT  (FRAME_BUFFER_MEMORY_AREA_HREZ * 4 * FRAME_BUFFER_MEMORY_AREA_VREZ)

static struct FrameBuffer_t frameBuffer;
static FrameBuffer fb = &frameBuffer;

static uint8_t frameBufferMemoryArea[FRAME_BUFFER_BYTE_COUNT];

int main( void ) {
    TestSuite suite = CreateTestSuite( "FrameBuffer" );
    if (!suite) return TestSuiteAbort( "Failed to create suite" );

    if (!memset(frameBufferMemoryArea, 0, FRAME_BUFFER_BYTE_COUNT))
        return TestSuiteAbort( "Failed to memset test frame buffer area" );

    Error e = PopulateFrameBuffer( fb, FRAME_BUFFER_MEMORY_AREA_HREZ, FRAME_BUFFER_MEMORY_AREA_VREZ, 32, &frameBufferMemoryArea );
    suite->AssertEquals->Int32( suite, NoError, e, "PopulateFrameBuffer()" );

    e = fb->DrawPixelAt( fb, 0, 0, 0x11223344 );
    suite->AssertEquals->Int32( suite, NoError, e, "DrawPixelAt(0, 0) return value" );

    TestSuitePictureMap pictureMap = CreateTestSuitePictureMap( 4 );
    pictureMap->Use4ByteEncoding( pictureMap )
              ->AddCharConversion( pictureMap, '*', 0x00223344 )
              ->AddCharConversion( pictureMap, ' ', 0x00000000 );

    uint8_t* byteArray = pictureMap->ConvertPictureToByteArray( pictureMap, 
        "*           "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            ", FRAME_BUFFER_PIXEL_COUNT );

    suite->AssertEquals->ByteArray( suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "DrawPixelAt(0, 0) byte comparison");

    e = fb->DrawLineAt( fb, 8, 5, 10, 0x00223344 );
    suite->AssertEquals->Int32( suite, NoError, e, "DrawLineAt(8, 5, 10) return value" );

    byteArray = pictureMap->ConvertPictureToByteArray( pictureMap,
        "*           "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "     *******"
        "***         ", FRAME_BUFFER_PIXEL_COUNT );

    suite->AssertEquals->ByteArray( suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "DrawLineAt(8, 5, 10) byte comparison");

    fb->FillAllWith( fb, 0x00000000 );

    byteArray = pictureMap->ConvertPictureToByteArray( pictureMap,
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            ", FRAME_BUFFER_PIXEL_COUNT );

    suite->AssertEquals->ByteArray( suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "FilAlllWith() byte comparison");

    e = fb->DrawAligned2ColorBitmapAt( fb, 1, 1, &(struct FrameBufferAligned2ColorBitmap_t){ 
        .PixelsPerRow = 8,
        .NumberOfRows = 3,
        .BitmapDefinition = (uint8_t[]){ 0xff, 0xc3, 0xff },
    }, 0x00223344, 0x00000000 );

    suite->AssertEquals->Int32( suite, NoError, e, "DrawAligned2ColorBitmapAt( 1, 1, 'square'... ) return value" );

    byteArray = pictureMap->ConvertPictureToByteArray( pictureMap,
        "            "
        " ********   "
        " **    **   "
        " ********   "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            ", FRAME_BUFFER_PIXEL_COUNT );

    suite->AssertEquals->ByteArray( suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "DrawAligned2ColorBitmapAt( 1, 1, 'square'...) byte comparison" );

    e = fb->ShiftPixelRowsUp( fb, 2, 2, 2 );

    suite->AssertEquals->Int32(suite, NoError, e, "First ShiftPixelRowsUp() return value");

    byteArray = pictureMap->ConvertPictureToByteArray(pictureMap,
        " **    **   "
        " ********   "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            "
        "            ",
        FRAME_BUFFER_PIXEL_COUNT);

    suite->AssertEquals->ByteArray(suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "First ShiftPixelRowsUp() byte comparison");

    fb->FillAllWith(fb, 0x00223344);

    byteArray = pictureMap->ConvertPictureToByteArray(pictureMap,
        "************"
        "************"
        "************"
        "************"
        "************"
        "************"
        "************"
        "************"
        "************"
        "************",
        FRAME_BUFFER_PIXEL_COUNT);

    suite->AssertEquals->ByteArray(suite, byteArray, frameBufferMemoryArea, FRAME_BUFFER_BYTE_COUNT, "First FillAllWith() byte comparison");

    //    char* pictureToPrint = TestSuiteConvertByteArrayToPicture( frameBufferMemoryArea, 4, FRAME_BUFFER_MEMORY_AREA_HREZ, FRAME_BUFFER_MEMORY_AREA_VREZ, (TestSuitePictureMapElement_t[]){
    //        (TestSuitePictureMapElement_t){ ' ', 0x00000000 },
    //        (TestSuitePictureMapElement_t){ '*', 0x00223344 },
    //    }, 2 );
    //
    //    printf( "%s", pictureToPrint );

    return suite->Done( suite );
}
