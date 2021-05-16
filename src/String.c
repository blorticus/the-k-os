#include <String.h>

static void reverseRunesInBuffer( RuneStringBuffer buffer, unsigned int numberOfRunesInBuffer ) {
    unsigned int numberOfRotations = numberOfRunesInBuffer / 2;
    unsigned int i, j;
    RuneString bp = buffer->String;
    Rune t;

    for (i = 0, j = numberOfRunesInBuffer - 1; i < numberOfRotations; i++, j--) {
        t = bp[i];
        bp[i] = bp[j];
        bp[j] = t;
    }
}

Error Uint64ToDecimalString( RuneStringBuffer inBuffer, uint64_t v ) {
    if (inBuffer->Size < 21)
        return ErrorBufferTooSmall;

    RuneString bufferPtr = (RuneString)inBuffer->String;
    unsigned int bi;
    unsigned int numberOfRunes = 0;

    if (v == 0) {
        bufferPtr[0] = U'0';
        bufferPtr[1] = 0;
        return NoError;
    }

    for (bi = 0; v > 0; bi++, numberOfRunes++)
    {
        bufferPtr[bi] = U'0' + (v % 10);
        v /= 10;
    }

    reverseRunesInBuffer( inBuffer, numberOfRunes );

    bufferPtr[bi] = 0;

    return NoError;
}

Error Int64ToDecimalString( RuneStringBuffer inBuffer, int64_t v ) {
    RuneStringBuffer_t tBuf;

    if (v >= 0)
        return Uint64ToDecimalString( inBuffer, v );

    tBuf.Size = inBuffer->Size - 1;
    tBuf.String = inBuffer->String + 1;

    Error e = Uint64ToDecimalString( &tBuf, v );

    if (e != NoError)
        return e;

    inBuffer->String[0] = U'-';

    return NoError;
}

Error Uint64ToZeroPaddedHexString( RuneStringBuffer inBuffer, uint64_t v, unsigned int minimumOutputFieldLength ) {
    if (inBuffer->Size < 17)
        return ErrorBufferTooSmall;

    RuneString bufferPtr = (RuneString)inBuffer->String;
    //unsigned int bi;
    unsigned int unshiftedNybbles = 64 / 4;
    Rune n;

    if (v == 0 && minimumOutputFieldLength == 0) {
        bufferPtr[0] = U'0';
        bufferPtr[1] = 0;
        return NoError;
    }

    for (unsigned int i = 0; i < 64 / 4 - minimumOutputFieldLength; i++)
    {
        if (v & 0xf000000000000000)
            break;
        v <<= 4;
        unshiftedNybbles--;
    }

    for ( ; unshiftedNybbles > 0; unshiftedNybbles--)
    {
        n = (v & 0xf000000000000000) >> 60;
        if (n > 9)
            *bufferPtr++ = U'a' + (n - 10);
        else
            *bufferPtr++ = U'0' + n;

        v <<= 4;
    }

    *bufferPtr = 0;

    return NoError;
}

Error Uint64ToHexString( RuneStringBuffer inBuffer, uint64_t v ) {
    return Uint64ToZeroPaddedHexString( inBuffer, v, 0 );
}

Error PopulateStringFormatter( StringFormatter f ) {
    //f->FormatIntoBuffer = FormatIntoBuffer;
    f->Uint64ToDecimalString = Uint64ToDecimalString;
    f->Int64ToDecimalString = Int64ToDecimalString;
    f->Uint64ToHexString = Uint64ToHexString;
    f->Uint64ToZeroPaddedHexString = Uint64ToZeroPaddedHexString;

    return NoError;
}

Error DecimalIntegerAsStringToInt64(const RuneString buffer, unsigned int maximumBufferLength, int64_t *value) {
    RuneString b = buffer;
    *value = 0;
    int numberIsNegative = 0;

    if (b[0] == 0)
        return ErrorNullValue;

    if (b[0] == U'-') {
        b++;
        numberIsNegative = 1;
    }

    if (!*b)  // negative sign alone
        return ErrorNotANumber;

    for (unsigned int i = 0; i < maximumBufferLength && b[i]; i++) {
        if (b[i] < U'0' || b[i] > U'9')
            return ErrorNotANumber;

        *value = *value * 10 + (b[i] - U'0');
    }

    if (numberIsNegative)
        *value = *value * -1;

    return NoError;
}