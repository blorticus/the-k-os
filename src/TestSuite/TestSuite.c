#include <TestSuite.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static int TestSuite_AssertEqualsUint8( TestSuite ts, uint8_t expect, uint8_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsUint16( TestSuite ts, uint16_t expect, uint16_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsUint32( TestSuite ts, uint32_t expect, uint32_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsUint64( TestSuite ts, uint64_t expect, uint64_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsInt8( TestSuite ts, int8_t expect, int8_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsInt16( TestSuite ts, int16_t expect, int16_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsInt32( TestSuite ts, int32_t expect, int32_t got, char* testName ) {
    return 0;
}

static int TestSuite_AssertEqualsInt64( TestSuite ts, int64_t expect, int64_t got, char* testName ) {
    return 0;
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
    return ts->numberOfFailedTests == 0;
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

static uint8_t* TestSuitePictureMap_ConvertPictureToByteArray( TestSuitePictureMap map, const char* picture, unsigned int pictureLength ) {
    uint8_t* byteArray = calloc( map->numberOfBytesInCoversions, pictureLength );
    if (!byteArray)
        return 0;

    uint8_t* bac = byteArray;

    for (unsigned int i = 0; i < pictureLength; i ++) {
        uint8_t* correspondingBytes = map->mapOfAsciiToConversionBytes[picture[i]];
        for (unsigned int j = 0; j < map->numberOfBytesInCoversions; j++)
            *bac++ = correspondingBytes[j];
    }

    return byteArray;
}


static TestSuitePictureMap TestSuitePictureMap_AddCharConversion( TestSuitePictureMap map, char c, ... ) {
    va_list byteSet;

    va_start(byteSet, c);

    uint8_t* store = calloc( 1, map->numberOfBytesInCoversions );

    for (unsigned int i = 0; i < map->numberOfBytesInCoversions; i++)
        store[i] = va_arg(byteSet, unsigned int);
        map->mapOfAsciiToConversionBytes[(uint8_t)c] = store;

    return map;
}


TestSuitePictureMap CreateTestSuitePictureMap( unsigned int numberOfBytesInConversion ) {
    TestSuitePictureMap map;

    if (!(map = malloc( sizeof( struct TestSuitePictureMap_t ) )))
        return 0;

    map->mapOfAsciiToConversionBytes = calloc( sizeof(uint8_t*), 256 );
    if (!map->mapOfAsciiToConversionBytes)
        return 0;

    map->numberOfBytesInCoversions = numberOfBytesInConversion;

    map->AddCharConversion = TestSuitePictureMap_AddCharConversion;
    map->ConvertPictureToByteArray = TestSuitePictureMap_ConvertPictureToByteArray;

    return map;
}

