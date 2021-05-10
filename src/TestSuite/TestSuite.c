#include <TestSuite.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static int TestSuite_AssertEqualsUint8( TestSuite ts, uint8_t expect, uint8_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%u / 0x%02x), got (%u / 0x%02x)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsUint16( TestSuite ts, uint16_t expect, uint16_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%u / 0x%04x), got (%u / 0x%04x)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsUint32( TestSuite ts, uint32_t expect, uint32_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%lu / 0x%08lx), got (%lu / 0x%08lx)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsUint64( TestSuite ts, uint64_t expect, uint64_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%llu / 0x%016llx), got (%lu / 0x%016llx)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsInt8( TestSuite ts, int8_t expect, int8_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%d / 0x%02x), got (%d / 0x%02x)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsInt16( TestSuite ts, int16_t expect, int16_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%d / 0x%04x), got (%d / 0x%04x)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsInt32( TestSuite ts, int32_t expect, int32_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%ld / %08lx), got (%ld / %08lx)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsInt64( TestSuite ts, int64_t expect, int64_t got, char* testName ) {
    if (expect == got)
        return ts->TestSucceeded( ts, testName );
    else
        return ts->TestFailed( ts, testName, "Expected (%lld / %016llx), got (%lld / %016llx)", expect, expect, got, got );
}

static int TestSuite_AssertEqualsByteArray( TestSuite ts, void* expect, void* got, unsigned int byteCount, char* testName ) {
    if (expect == 0)
        if (got == 0)
            return ts->TestSucceeded( ts, testName );
        else
            return ts->TestFailed( ts, testName, "Expect null array; got non-null array" );

    if (got == 0)
        return ts->TestFailed( ts, testName, "Expected non-null array; got null array" );

    uint8_t* expectByte = (uint8_t*)expect;
    uint8_t* gotByte    = (uint8_t*)got;

    for (unsigned int i = 0; i < byteCount; i++)
        if (expectByte[i] != gotByte[i])
            return ts->TestFailed( ts, testName, "At array byte (%i), expected (0x%02x), got (0x%02x)", i, expectByte[i], gotByte[i] );

    return ts->TestSucceeded( ts, testName );
}

static int TestSuite_TestFailed( TestSuite ts, char* testName, char* msg, ... ) {
    va_list messageArgs;
    va_start( messageArgs, msg );

    char* expandedMessage = malloc( 2048 );
    vsnprintf( expandedMessage, 2048, msg, messageArgs );
    
    ts->numberOfFailedTests++;

    printf( "[%i] [%s] -- FAILED [%s]\n", (ts->numberOfFailedTests + ts->numberOfSuccessfulTests), testName, expandedMessage );

    free( expandedMessage );

    return 0;
}

static int TestSuite_TestSucceeded( TestSuite ts, char* testName ) {
    ts->numberOfSuccessfulTests++;
    return 1;
}

static int TestSuite_Done( TestSuite ts ) {
    printf( "Ran [%i] Tests; [%i] Successful; [%i] Failed -- %s\n", ts->numberOfSuccessfulTests + ts->numberOfFailedTests, ts->numberOfSuccessfulTests, ts->numberOfFailedTests, (ts->numberOfFailedTests > 0 ? "NOT OK" : "OK") );
    return ts->numberOfFailedTests;
}

TestSuite CreateTestSuite( char* suiteName ) {
    TestSuite ts = malloc( sizeof( struct TestSuite_t ) );
    if (!ts) return 0;

    TestSuiteAssertEquals e = malloc( sizeof( struct TestSuiteAssertEquals_t ) );
    if (!e) return 0;

    e->Uint8  = TestSuite_AssertEqualsUint8;
    e->Uint16 = TestSuite_AssertEqualsUint16;
    e->Uint32 = TestSuite_AssertEqualsUint32;
    e->Uint64 = TestSuite_AssertEqualsUint64;
    e->Int8   = TestSuite_AssertEqualsInt8;
    e->Int16  = TestSuite_AssertEqualsInt16;
    e->Int32  = TestSuite_AssertEqualsInt32;
    e->Int64  = TestSuite_AssertEqualsInt64;
    e->ByteArray = TestSuite_AssertEqualsByteArray;

    ts->AssertEquals  = e;

    ts->TestSucceeded = TestSuite_TestSucceeded;
    ts->TestFailed    = TestSuite_TestFailed;
    ts->Done          = TestSuite_Done;

    ts->numberOfSuccessfulTests = 0;
    ts->numberOfFailedTests     = 0;

    return ts;
}

static void* TestSuitePictureMap_ConvertPictureToByteArray( TestSuitePictureMap map, const char* picture, unsigned int pictureLength ) {
    void* byteArray = calloc( map->numberOfBytesForEncoding, pictureLength );
    if (!byteArray) return 0;

    uint8_t* p8;
    uint16_t* p16;
    uint32_t* p32;
    uint64_t* p64;

    switch (map->numberOfBytesForEncoding) {
        case 1:
            p8 = byteArray;
            for (unsigned int i = 0; i < pictureLength; i++) {
                uint8_t b = (uint8_t)map->mapOfAsciiToConversionBytes[picture[i]];
                *p8++ = b;
            }
            break;

        case 2:
            p16 = byteArray;
            for (unsigned int i = 0; i < pictureLength; i++) {
                uint16_t b = (uint16_t)map->mapOfAsciiToConversionBytes[picture[i]];
                *p16++ = b;
            }
            break;

        case 4:
            p32 = byteArray;
            for (unsigned int i = 0; i < pictureLength; i++) {
                uint32_t b = (uint32_t)map->mapOfAsciiToConversionBytes[picture[i]];
                *p32++ = b;
            }
            break;

        case 8:
            p64 = byteArray;
            for (unsigned int i = 0; i < pictureLength; i++) {
                uint64_t b = (uint64_t)map->mapOfAsciiToConversionBytes[picture[i]];
                *p64++ = b;
            }
            break;
    }

    return byteArray;
}


static TestSuitePictureMap TestSuitePictureMap_Use1ByteEncoding( TestSuitePictureMap map ) {
    map->numberOfBytesForEncoding = 1;
}

static TestSuitePictureMap TestSuitePictureMap_Use2ByteEncoding( TestSuitePictureMap map ) {
    map->numberOfBytesForEncoding = 2;
}

static TestSuitePictureMap TestSuitePictureMap_Use4ByteEncoding( TestSuitePictureMap map ) {
    map->numberOfBytesForEncoding = 4;
}

static TestSuitePictureMap TestSuitePictureMap_Use8ByteEncoding( TestSuitePictureMap map ) {
    map->numberOfBytesForEncoding = 8;
}

static TestSuitePictureMap TestSuitePictureMap_AddCharConversion( TestSuitePictureMap map, char c, uint64_t encodedValue ) {
    map->mapOfAsciiToConversionBytes[(uint8_t)c] = encodedValue;
    return map;
}

TestSuitePictureMap CreateTestSuitePictureMap() {
    TestSuitePictureMap map;

    if (!(map = malloc( sizeof( struct TestSuitePictureMap_t ) )))
        return 0;

    map->mapOfAsciiToConversionBytes = calloc( sizeof(uint64_t), 256 );
    if (!map->mapOfAsciiToConversionBytes)
        return 0;

    map->numberOfBytesForEncoding = 8;

    map->AddCharConversion = TestSuitePictureMap_AddCharConversion;
    map->ConvertPictureToByteArray = TestSuitePictureMap_ConvertPictureToByteArray;

    map->Use1ByteEncoding = TestSuitePictureMap_Use1ByteEncoding;
    map->Use2ByteEncoding = TestSuitePictureMap_Use2ByteEncoding;
    map->Use4ByteEncoding = TestSuitePictureMap_Use4ByteEncoding;
    map->Use8ByteEncoding = TestSuitePictureMap_Use8ByteEncoding;

    return map;
}

int TestSuiteAbort( const char* message ) {
    fprintf( stderr, "TEST SUITE ABORTED: %s\n", message );
    return (-1);
}

char* TestSuiteConvertByteArrayToPicture( void* byteArray, unsigned int bytesPerPixel, unsigned int pixelsPerRow, unsigned int rowsInPicture, TestSuitePictureMapElements mappings, unsigned int numberOfMappings ) {
    uint8_t*  p8;
    uint16_t* p16;
    uint32_t* p32;
    uint64_t* p64;

    char* _picture = calloc( (pixelsPerRow + 1) * rowsInPicture + 1, bytesPerPixel );
    char* picture = _picture;

    switch (bytesPerPixel) {
        case 1:
            p8 = (uint8_t*)byteArray;

            for (unsigned int row = 0; row < rowsInPicture; row++) {
                for (unsigned int pixelInRow = 0; pixelInRow < pixelsPerRow; pixelInRow++) {
                    uint8_t b = *p8++;

                    char printChar = '?';
                    for (unsigned int i = 0; i < numberOfMappings; i++) {
                        if (b == (uint8_t)(mappings[i].CorrespondingBytes)) {
                            printChar = mappings[i].PictureCharacter;
                            break;
                        }
                    }

                    *picture++ = printChar;
                }

                *picture++ = '\n';
            }

            break;

        case 2:
            p16 = (uint16_t*)byteArray;

            for (unsigned int row = 0; row < rowsInPicture; row++) {
                for (unsigned int pixelInRow = 0; pixelInRow < pixelsPerRow; pixelInRow++) {
                    uint16_t b = *p16++;

                    char printChar = '?';
                    for (unsigned int i = 0; i < numberOfMappings; i++) {
                        if (b == (uint16_t)(mappings[i].CorrespondingBytes)) {
                            printChar = mappings[i].PictureCharacter;
                            break;
                        }
                    }

                    *picture++ = printChar;
                }

                *picture++ = '\n';
            }

            break;


        case 4:
            p32 = (uint32_t*)byteArray;

            for (unsigned int row = 0; row < rowsInPicture; row++) {
                for (unsigned int pixelInRow = 0; pixelInRow < pixelsPerRow; pixelInRow++) {
                    uint32_t b = *p32++;

                    char printChar = '?';
                    for (unsigned int i = 0; i < numberOfMappings; i++) {
                        if (b == (uint32_t)(mappings[i].CorrespondingBytes)) {
                            printChar = mappings[i].PictureCharacter;
                            break;
                        }
                    }

                    *picture++ = printChar;
                }

                *picture++ = '\n';
            }

            break;


        case 8:
            p64 = (uint64_t*)byteArray;

            for (unsigned int row = 0; row < rowsInPicture; row++) {
                for (unsigned int pixelInRow = 0; pixelInRow < pixelsPerRow; pixelInRow++) {
                    uint64_t b = *p64++;

                    char printChar = '?';
                    for (unsigned int i = 0; i < numberOfMappings; i++) {
                        if (b == (uint64_t)(mappings[i].CorrespondingBytes)) {
                            printChar = mappings[i].PictureCharacter;
                            break;
                        }
                    }

                    *picture++ = printChar;
                }

                *picture++ = '\n';
            }

            break;


        default:
            return 0;
    }

    return _picture;
}
