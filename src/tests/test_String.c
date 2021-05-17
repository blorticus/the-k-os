#include <String.h>
#include <TestSuite.h>
#include <stdio.h>
#include <stdlib.h>

void testDecimalIntegerAsStringToInt64( TestSuite suite );
void testUint64ToHexString( TestSuite suite );
void testUint64ToHexStringPadded( TestSuite suite );

int main(void)
{
    TestSuite suite = CreateTestSuite( "String" );

    testDecimalIntegerAsStringToInt64( suite );
    testUint64ToHexString( suite );
    testUint64ToHexStringPadded( suite );

    return suite->Done( suite );
}

typedef struct DecimalIntegerAsStringToInt64_test {
    RuneString string;
    int64_t integerValue;
    Error expectedError;
} DecimalIntegerAsStringToInt64_test;

void testDecimalIntegerAsStringToInt64( TestSuite suite )
{
    DecimalIntegerAsStringToInt64_test DecimalIntegerAsStringToInt64_testSet[] = {
        { U"", 0, ErrorNullValue },
        { U"x", 0, ErrorNotANumber },
        { U"-", 0, ErrorNotANumber },
        { U"-x", 0, ErrorNotANumber },
        { U"0", 0, NoError },
        { U"00", 0, NoError },
        { U"-0", 0, NoError },
        { U"1", 1, NoError },
        { U"-1", -1, NoError },
        { U"10", 10, NoError },
        { U"-10", -10, NoError },
        { U"11", 11, NoError },
        { U"-11", -11, NoError },
        { U"23456789012", 23456789012, NoError },
        { U"-23456789012", -23456789012, NoError },
        { U"11x", 11, ErrorNotANumber },
        { U"-11x", -11, ErrorNotANumber },
        { U"234567x89012", 23456789012, ErrorNotANumber },
        { U"-234567x89012", -23456789012, ErrorNotANumber },
    };
    
    Error e;
    int64_t x;
    char testname[1024];

    for (int i = 0; i < (int)(sizeof(DecimalIntegerAsStringToInt64_testSet) / sizeof(DecimalIntegerAsStringToInt64_test)); i++)
    {
        DecimalIntegerAsStringToInt64_test t = DecimalIntegerAsStringToInt64_testSet[i];
        e = DecimalIntegerAsStringToInt64( t.string, 1024, &x );

        sprintf( testname, "(testDecimalIntegerAsStringToInt64) DecimalIntegerAsStringToInt64 test (%i) error check", i );
        suite->AssertEquals->Int32( suite, t.expectedError, e, testname );

        if (t.expectedError == NoError) {
            sprintf( testname, "(testDecimalIntegerAsStringToInt64) DecimalIntegerAsStringToInt64 test (%i) value check", i );
            suite->AssertEquals->Int64( suite, x, t.integerValue, testname );
        }
    }
}

typedef struct Uint64ToHexString_test {
    uint64_t intValue;
    const RuneString expectedString;
} Uint64ToHexString_test;

void testUint64ToHexString( TestSuite suite ) {
    Rune runeBuffer[30];
    struct RuneStringBuffer_t inBuffer = {
        .Size = 30,
        .String = runeBuffer,
    };
    char* testname = calloc( 1024, 1 );

    Uint64ToHexString_test Uint64ToHexString_testset[] = {
        { 0x0, U"0" },
        { 0x1, U"1" },
        { 0xa, U"a" },
        { 0xf, U"f" },
        { 0x1f, U"1f" },
        { 0xff, U"ff" },
        { 0xf0, U"f0" },
        { 0x1234af987, U"1234af987" },
        { 0xffffffffffffffff, U"ffffffffffffffff" },
    };

    Error e;
    for (size_t i = 0; i < sizeof(Uint64ToHexString_testset) / sizeof(Uint64ToHexString_test); i++) {
        Uint64ToHexString_test t = Uint64ToHexString_testset[i];

        e = Uint64ToHexString( &inBuffer, t.intValue );

        snprintf( testname, 1024, "(testUint64ToHexString) Test (%lu) error check", i + 1 );
        suite->AssertEquals->Int32( suite, NoError, e, testname );

        snprintf( testname, 1024, "(testUint64ToHexString) Test (%lu) value check", i + 1 );
        suite->AssertEquals->RuneString( suite, t.expectedString, (const RuneString)inBuffer.String, inBuffer.Size, testname );

    }
}

typedef struct Uint64ToHexStringPadded_test {
    uint64_t intValue;
    const RuneString expectedString;
    unsigned int minimumFieldLength;
} Uint64ToHexStringPadded_test;

void testUint64ToHexStringPadded( TestSuite suite ) {
    Rune runeBuffer[30];
    struct RuneStringBuffer_t inBuffer = {
        .Size = 30,
        .String = runeBuffer,
    };
    char* testname = calloc( 1024, 1 );

    Uint64ToHexStringPadded_test Uint64ToHexStringPadded_testset[] = {
        { 0x0, U"0", 1 },
        { 0x0, U"00", 2 },
        { 0x0, U"0000000", 7 },
        { 0x0, U"000000000000000", 15 },
        { 0x0, U"0000000000000000", 16 },
        { 0x1, U"1", 1 },
        { 0x1, U"01", 2 },
        { 0x1, U"0001", 4 },
        { 0x1, U"000000001", 9 },
        { 0x1, U"0000000000000001", 16 },
        { 0xa, U"a", 0 },
        { 0xa, U"a", 1 },
        { 0xa, U"0000000a", 8 },
        { 0xf, U"f", 0 },
        { 0xf, U"f", 1 },
        { 0xf, U"0000000000000f", 14 },
        { 0xf, U"000000000000000f", 16 },
        { 0x1f, U"1f", 0 },
        { 0x1f, U"1f", 1 },
        { 0x1f, U"1f", 2 },
        { 0x1f, U"0001f", 5 },
        { 0xff, U"ff", 0 },
        { 0xff, U"ff", 1 },
        { 0xff, U"00000000ff", 10 },
        { 0xf0, U"f0", 0 },
        { 0xf0, U"f0", 1 },
        { 0xf0, U"f0", 2 },
        { 0xf0, U"0f0", 3 },
        { 0x1234af987, U"1234af987", 0 },
        { 0x1234af987, U"1234af987", 1 },
        { 0x1234af987, U"1234af987", 2 },
        { 0x1234af987, U"1234af987", 9 },
        { 0x1234af987, U"01234af987", 10 },
        { 0x1234af987, U"001234af987", 11 },
        { 0x1234af987, U"00000001234af987", 16 },
        { 0xffffffffffffffff, U"ffffffffffffffff", 0 },
        { 0xffffffffffffffff, U"ffffffffffffffff", 1 },
        { 0xffffffffffffffff, U"ffffffffffffffff", 10 },
        { 0xffffffffffffffff, U"ffffffffffffffff", 16 },
    };

    Error e;
    for (size_t i = 0; i < sizeof(Uint64ToHexStringPadded_testset) / sizeof(Uint64ToHexStringPadded_test); i++) {
        Uint64ToHexStringPadded_test t = Uint64ToHexStringPadded_testset[i];

        e = Uint64ToZeroPaddedHexString( &inBuffer, t.intValue, t.minimumFieldLength );

        snprintf( testname, 1024, "(testUint64ToHexStringPadded) Test (%lu) error check", i + 1 );
        suite->AssertEquals->Int32( suite, NoError, e, testname );

        snprintf( testname, 1024, "(testUint64ToHexStringPadded) Test (%lu) value check", i + 1 );
        suite->AssertEquals->RuneString( suite, t.expectedString, (const RuneString)inBuffer.String, inBuffer.Size, testname );
    }
}
