#include <String.h>
#include <TestSuite.h>
#include <stdio.h>
#include <stdlib.h>

void testDecimalIntegerAsStringToInt64( TestSuite suite );
void testUint64ToHexString( TestSuite suite );
void testUint64ToHexStringPadded( TestSuite suite );
void testFormatIterativelyIntoBuffer( TestSuite suite );
void testFormatIntoBuffer( TestSuite suite );

int main(void)
{
    TestSuite suite = CreateTestSuite( "String" );

    testUint64ToHexString( suite );
    testUint64ToHexStringPadded( suite );
    testDecimalIntegerAsStringToInt64( suite );
    testFormatIterativelyIntoBuffer( suite );
    testFormatIntoBuffer( suite );

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

typedef struct formatIterativelyIntoBufferExpectSet_t {
    Error ExpectedError;
    RuneString ExpectedBufferString;
    int ExpectThisToBeLastChunk;
} formatIterativelyIntoBufferExpectSet_t;

typedef struct formatIterativelyIntoBufferTest_t {
    formatIterativelyIntoBufferExpectSet_t* ExpectedChunkResults;
    unsigned int NumberOfExpectedChunkResults;
    unsigned int IndexOfNextExpectedChunk;
    unsigned int NumberOfChunksProduced;
    int AllChunksMatchExpectedValues;
    int IndexOfFirstChunkThatDoesNotMatch;
    char additionalFailureDiagnosticMessage[256];
} formatIterativelyIntoBufferTest_t;

static void resetFormatIterativelyIntoBufferTest( formatIterativelyIntoBufferTest_t* t, formatIterativelyIntoBufferExpectSet_t* expectedChunkResults, unsigned int numberOfExpectedChunkResults )
{
    t->ExpectedChunkResults = expectedChunkResults;
    t->NumberOfExpectedChunkResults = numberOfExpectedChunkResults;
    t->NumberOfChunksProduced = 0;
    t->AllChunksMatchExpectedValues = 1;
    t->IndexOfNextExpectedChunk = 0;
    t->IndexOfFirstChunkThatDoesNotMatch = 0;
    t->additionalFailureDiagnosticMessage[0] = 0;
}

static int setFormatIterativelyIntoBufferTestErrorValues( formatIterativelyIntoBufferTest_t* t, unsigned int indexOfFirstChunkThatDoesNotMatch, char* additionalFailureDiagnosticMessageFormat, ... )
{
    va_list varargs;
    va_start( varargs, additionalFailureDiagnosticMessageFormat );
    t->AllChunksMatchExpectedValues = 0;
    t->IndexOfFirstChunkThatDoesNotMatch = indexOfFirstChunkThatDoesNotMatch;
    vsnprintf( t->additionalFailureDiagnosticMessage, 256, additionalFailureDiagnosticMessageFormat, varargs );
    va_end( varargs );
    return 1;
}

static int strncmp32( RuneString left, RuneString right, unsigned int maximumLength )
{
    if (!left)
    {
        if (!right)
            return 0;
        else
            return 1;
    }

    if (!right)
        return -1;

    for (unsigned int i = 0; i < maximumLength; i++)
    {
        if (left[i] < right[i])
            return 1;

        if (left[i] > right[i])
            return -1;

        if (!left[i])
            return 0;
    }

    return 0;
}

int iteratingCallback( RuneStringBuffer inBuffer, Error e, int done, void* additionalArgs )
{
    formatIterativelyIntoBufferTest_t* testSet = (formatIterativelyIntoBufferTest_t*)additionalArgs;

    testSet->NumberOfChunksProduced++;

    if (testSet->NumberOfChunksProduced > testSet->NumberOfExpectedChunkResults)
        return setFormatIterativelyIntoBufferTestErrorValues( testSet, testSet->NumberOfChunksProduced, "Processed more chunks than expected" );

    formatIterativelyIntoBufferExpectSet_t expectedResultsForThisChunk = testSet->ExpectedChunkResults[testSet->IndexOfNextExpectedChunk];

    if (expectedResultsForThisChunk.ExpectThisToBeLastChunk)
    {
        if (!done)
        {
            return setFormatIterativelyIntoBufferTestErrorValues( testSet, testSet->IndexOfNextExpectedChunk, "Expected to be done but not done" );
        }
    }
    else if (done)
    {
        return setFormatIterativelyIntoBufferTestErrorValues( testSet, testSet->IndexOfNextExpectedChunk, "Expected to not be done, but callback indicated done" );
    }

    if (expectedResultsForThisChunk.ExpectedError != e)
        return setFormatIterativelyIntoBufferTestErrorValues( testSet, testSet->IndexOfNextExpectedChunk, "Expected error = (%d), got error = (%d)", expectedResultsForThisChunk.ExpectedError, e );

    if (strncmp32( expectedResultsForThisChunk.ExpectedBufferString, inBuffer->String, inBuffer->Size ))
        return setFormatIterativelyIntoBufferTestErrorValues( testSet, testSet->IndexOfNextExpectedChunk, "String does not match expected value" );

    testSet->IndexOfNextExpectedChunk++;

    return 0;
}

static int validateFormatIterativelyIntoBufferResults( TestSuite suite, formatIterativelyIntoBufferTest_t* testSet, char* testName )
{

    if (testSet->AllChunksMatchExpectedValues)
    {
        if (testSet->NumberOfChunksProduced < testSet->NumberOfExpectedChunkResults)
            return suite->TestFailed( suite, testName, "Expected (%i) chunks but only (%i) chunks were generated", testSet->NumberOfExpectedChunkResults, testSet->NumberOfChunksProduced);

        return suite->TestSucceeded( suite, testName );
    }

    return suite->TestFailed( suite, testName, "on chunk (%i) %s", testSet->IndexOfFirstChunkThatDoesNotMatch, testSet->additionalFailureDiagnosticMessage );
}

void testFormatIterativelyIntoBuffer( TestSuite suite )
{
    StringFormatter f = calloc( 1, sizeof( struct StringFormatter_t ) );
    Error e = PopulateStringFormatter( f );

    suite->AssertEquals->Int32( suite, NoError, e, "PopulateStringFormatter() return value" );

    RuneStringBuffer inBuffer = calloc( 1, sizeof( RuneStringBuffer_t ) );
    RuneString stringBuffer = calloc( 64, sizeof( Rune ) );
    inBuffer->String = stringBuffer;
    inBuffer->Size = 64;

    formatIterativelyIntoBufferTest_t testSet;

    formatIterativelyIntoBufferExpectSet_t expectedChunkResults01[] = {
        { .ExpectedBufferString = U"", .ExpectedError = NoError, .ExpectThisToBeLastChunk = 1 },
    };
    resetFormatIterativelyIntoBufferTest( &testSet, expectedChunkResults01, sizeof(expectedChunkResults01) / sizeof(formatIterativelyIntoBufferExpectSet_t) );

    FormatBufferIteratingCallback callback = calloc( 1, sizeof( struct FormatBufferIteratingCallback_t ) );
    callback->OnNextFormatChunk = iteratingCallback;
    callback->AdditionalArgs = (void*)&testSet;

    f->FormatIterativelyIntoBuffer( inBuffer, callback, U"" );
    validateFormatIterativelyIntoBufferResults( suite, &testSet, "FormatIterativelyIntoBuffer('')" );

    formatIterativelyIntoBufferExpectSet_t expectedChunkResults02[] = {
        { .ExpectedBufferString = U"testing", .ExpectedError = NoError, .ExpectThisToBeLastChunk = 1 },
    };
    resetFormatIterativelyIntoBufferTest( &testSet, expectedChunkResults02, sizeof(expectedChunkResults02) / sizeof(formatIterativelyIntoBufferExpectSet_t) );

    f->FormatIterativelyIntoBuffer( inBuffer, callback, U"testing" );
    validateFormatIterativelyIntoBufferResults( suite, &testSet, "FormatIterativelyIntoBuffer('testing')" );
}

void testFormatIntoBuffer( TestSuite suite )
{
    StringFormatter f = calloc( 1, sizeof( struct StringFormatter_t ) );
    Error e = PopulateStringFormatter( f );

    suite->AssertEquals->Int32( suite, NoError, e, "PopulateStringFormatter() return value" );

    RuneStringBuffer inBuffer = calloc( 1, sizeof( RuneStringBuffer_t ) );
    RuneString stringBuffer = calloc( 64, sizeof( Rune ) );
    inBuffer->String = stringBuffer;
    inBuffer->Size = 64;

    e = f->FormatIntoBuffer( f, inBuffer, U"" );
    suite->AssertEquals->Int32( suite, NoError, e, "FormatIntoBuffer('') return value" );
    suite->AssertEquals->RuneString( suite, U"", inBuffer->String, 1, "FormatIntoBuffer('') string comparison" );

    e = f->FormatIntoBuffer( f, inBuffer, U"testing" );
    suite->AssertEquals->Int32( suite, NoError, e, "FormatIntoBuffer('testing') return value" );
    suite->AssertEquals->RuneString( suite, U"testing", inBuffer->String, 8, "FormatIntoBuffer('testing') string comparison" );

}