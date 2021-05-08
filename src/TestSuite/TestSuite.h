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

    int (*ByteArray)( TestSuite ts, void* expect, void* got, unsigned int byteCount, char* testName );
} *TestSuiteAssertEquals;

typedef struct TestSuitePictureMapElement_t {
    char PictureCharacter;
    uint8_t* CorrespondingBytes;
} *TestSuitePictureMapElement, *TestSuitePictureMapElements, TestSuitePictureMapElement_t;

// TestSuitePictureMap is a pointer to a object used to create byte arrays from an ASCII
// picture.  This is intended to assist with pixel generators for FrameBuffers.  It is easier
// to understand the test (for humans) if the values are expressed in a visual format, rather
// than as a stream of bytes (which is the thing actually being compared in the test).  The
// constructor is:
//   TestSuitePictureMap CreateTestSuitePictureMap( unsigned int numberOfBytesInConversion );
//
// A "picture map" consists of ASCII characters.  Each character is mapped to a fixed set of
// bytes.  The number of bytes is the same for each character.  That is the
// 'numberOfBytesInConversion'.  For example:
//   map = CreateTestSuitePictureMap( 2 );
//   map->AddCharConversion( map, '-', 0x01, 0x02 )
//      ->AddCharConversion( map, '*', 0xaa, 0xbb );
//   const char[] picture = "------"
//                          "-*--*-"
//                          "-*--*-"
//                          "------"
//   uint8_t* a = map->ConvertPictureToByteArray( map, picture, 24 );
//
// would produce the following byte array (as 'a'):
//   { 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, // row 1
//     0x01, 0x02, 0xaa, 0xbb, 0x01, 0x02, 0x01, 0x02, 0xaa, 0xbb, 0x01, 0x02, // row 2
//     ... etc ... }
//
typedef struct TestSuitePictureMap_t {
    uint8_t** mapOfAsciiToConversionBytes;
    unsigned int numberOfBytesInCoversions;

    uint8_t* (*ConvertPictureToByteArray)( struct TestSuitePictureMap_t* map, const char* picture, unsigned int pictureLength );
    struct TestSuitePictureMap_t* (*AddCharConversion)( struct TestSuitePictureMap_t* map, char c, ... );
} *TestSuitePictureMap;

// TestSuite is a pointer to a TestSuite instance.  The typical usage pattern is:
//   TestSuite suite = CreateTestSuite( "FrameBuffer Tests" );
//   // Do tests ...
//   suite->Done( suite );
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
TestSuitePictureMap CreateTestSuitePictureMap( unsigned int numberOfBytesInConversion );
