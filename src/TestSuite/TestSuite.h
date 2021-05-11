#pragma once

/*
 * A simple unit testing suite for the K-OS.  It follows the same quasi OO pattern
 * as used in the kernel itself.  "Objects" are plain structs.  "Methods" are
 * function refs bound to each instance of the struct.  Unfortunately, the
 * struct ref must be passed as the first parameter of each method call, which
 * is unwieldy.
 * 
 * This library set uses a positive assertion model.  It also exports many 
 * functions that are part of the standard C library.  That's because many
 * parts of the kernel have symbols that overlap with the standard C library,
 * so to compile these tests, stdlib must be omitted.  To use those standard
 * functions to create unit tests, they must come from this statically
 * compiled library
 */

#include <stdint.h>

typedef struct TestSuite_t *TestSuite;

typedef struct TestSuiteAssertEquals_t {
    int (*Uint8)( TestSuite ts, uint8_t expect, uint8_t got, char* testName );
    int (*Uint16)( TestSuite ts, uint16_t expect, uint16_t got, char* testName );
    int (*Uint32)( TestSuite ts, uint32_t expect, uint32_t got, char* testName );
    int (*Uint64)( TestSuite ts, uint64_t expect, uint64_t got, char* testName );

    int (*Int8)( TestSuite ts, int8_t expect, int8_t got, char* testName );
    int (*Int16)( TestSuite ts, int16_t expect, int16_t got, char* testName );
    int (*Int32)( TestSuite ts, int32_t expect, int32_t got, char* testName );
    int (*Int64)( TestSuite ts, int64_t expect, int64_t got, char* testName );

    int (*ByteArray)( TestSuite ts, const void* expect, const void* got, unsigned int byteCount, char* testName );
} *TestSuiteAssertEquals;

typedef struct TestSuitePictureMapElement_t {
    char PictureCharacter;
    uint64_t CorrespondingBytes;
} *TestSuitePictureMapElement, *TestSuitePictureMapElements, TestSuitePictureMapElement_t;

// TestSuitePictureMap is a pointer to a object used to create byte arrays from an ASCII
// picture.  This is intended to assist with pixel generators for FrameBuffers.  It is easier
// to understand the test (for humans) if the values are expressed in a visual format, rather
// than as a stream of bytes (which is the thing actually being compared in the test).  The
// constructor is:
//   TestSuitePictureMap CreateTestSuitePictureMap();
//
// A "picture map" consists of ASCII characters.  Each character is mapped to a fixed set of
// bytes.  The number of bytes is the same for each character.  Before adding conversions or
// converting an image, the encoding size should be declared (the default is 8Byte):
//   map = CreateTestSuitePictureMap();
//   map->Use2ByteEncoding( map );
//
// Each character in the map will be expanded to a series of bytes based on the encoding.  To
// map a character to an encoding:
//   map->AddCharConversion( map, '-', 0x1122 )
//      ->AddCharConversion( map, '*', 0xaabb );
//
// The actual value will be cast according to the enconding.  For example, with 2Byte encoding,
// the bytes values above will be cast as uint16s.
//
// The next step is to pass a picture.  For example:
//   const char[] picture = "------"
//                          "-*--*-"
//                          "-*--*-"
//                          "------"
//   uint16_t* a = map->ConvertPictureToArray( map, picture, 24 );
//
// would produce the following array (as 'a'):
//   { 0x0102, 0x0102, 0x0102, 0x0102, 0x0102, 0x0102, // row 1
//     0x0102, 0xaabb, 0x0102, 0x0102, 0xaabb, 0x0102, // row 2
//     ... etc ... }
//
// It is up to you to ensure that you cast the pointer appropriately (i.e., uint8_t* for 1Byte,
// uint16_t* for 2Byte, and so forth).  This mechanism allows sensible comparisons regardless of 
// architecture endianness.
//
typedef struct TestSuitePictureMap_t {
    uint64_t* mapOfAsciiToConversionBytes;
    unsigned int numberOfBytesForEncoding;

    struct TestSuitePictureMap_t* (*AddCharConversion)( struct TestSuitePictureMap_t* map, char c, uint64_t v );

    struct TestSuitePictureMap_t* (*Use1ByteEncoding)( struct TestSuitePictureMap_t* map );
    struct TestSuitePictureMap_t* (*Use2ByteEncoding)( struct TestSuitePictureMap_t* map );
    struct TestSuitePictureMap_t* (*Use4ByteEncoding)( struct TestSuitePictureMap_t* map );
    struct TestSuitePictureMap_t* (*Use8ByteEncoding)( struct TestSuitePictureMap_t* map );

    void* (*ConvertPictureToByteArray)( struct TestSuitePictureMap_t* map, const char* picture, unsigned int pictureLength );
} *TestSuitePictureMap;

// TestSuite is a pointer to a TestSuite instance.  The typical usage pattern is:
//   TestSuite suite = CreateTestSuite( "FrameBuffer Tests" );
//   // Do tests ...
//   return suite->Done( suite );
//
// The tests follow this pattern:
//   suite->AssertEquals->Uint16( suite, expectedUint16Value, actualUint16Value, "some test name..." );
// AssertEquals methods exists for all UintN and IntN where N is 8, 16, 32 and 64.  A method also exists for
// byte-to-byte comparison of pointer values:
//   suite->AssertEquals->ByteArray( suite, void* expect, void* got, unsigned int byteCount, char* testName );
//
typedef struct TestSuite_t {
    char* name;

    unsigned int numberOfSuccessfulTests;
    unsigned int numberOfFailedTests;

    TestSuiteAssertEquals AssertEquals;

    int (*TestSucceeded)( TestSuite ts, char* testName );
    int (*TestFailed)( TestSuite ts, char* testName, char* msg, ... );
    int (*Done)( TestSuite ts );
} *TestSuite;


TestSuite CreateTestSuite( char* suiteName );
TestSuitePictureMap CreateTestSuitePictureMap( );
int TestSuiteAbort( const char* message );

char* TestSuiteConvertByteArrayToPicture( void* byteArray, unsigned int bytesPerPixel, unsigned int pixelsPerRow, unsigned int rowsInPicture, TestSuitePictureMapElements mapping, unsigned int numberOfMappings );
