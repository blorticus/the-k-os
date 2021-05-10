#include <stdio.h>
#include <TestSuite.h>

void main( void ) {
    TestSuite ts = CreateTestSuite( "test" );
    char l[] = { 0x00, 0x10, 0xff, 0x20 };
    char r[] = { 0x00, 0x10, 0xff, 0x21 };
    char r2[] = { 0x00, 0x10, 0xff, 0x20 };

    ts->AssertEquals->ByteArray( ts, l, NULL, 4, "r != NULL" );
    ts->AssertEquals->ByteArray( ts, l, l, 4, "l = l" );
    ts->AssertEquals->ByteArray( ts, l, r, 4, "l != r" );
    ts->AssertEquals->ByteArray( ts, l, r2, 4, "l = r" );

//    TestSuitePictureMap map = CreateTestSuitePictureMap( 4 );
//    map->AddCharConversion( map, '*', 0x11, 0x22, 0x33, 0x44 )
//       ->AddCharConversion( map, '-', 0x00, 0xff, 0xff, 0xff );
//
//    const char* p1 = "**--**"
//                     "--**--"
//                     "*-*-*-"
//                     "------"
//                     "******";
//
//    uint8_t* pb = map->ConvertPictureToByteArray( map, p1, 30 );
//
//    uint8_t e[] = {
//        0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, // 24
//        0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, // 48
//        0x11, 0x22, 0x33, 0x44, 0x00, 0xff, 0xff, 0xff, 0x11, 0x22, 0x33, 0x44, 0x00, 0xff, 0xff, 0xff, 0x11, 0x22, 0x33, 0x44, 0x00, 0xff, 0xff, 0xff, // 72
//        0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, // 96
//        0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44, // 120
//    };
//
//    for (int i = 0; i < 24 * 5; i++) {
//        if (pb[i] != e[i])
//            printf( "Failed at byte %i\n", i );
//    }

    ts->Done( ts );
}
