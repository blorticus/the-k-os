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

typedef enum {
    StringFormatSequenceModifer_Length      = 0x01,
    StringFormatSequenceModifer_Truncate    = 0x02,
    StringFormatSequenceModifer_Fill        = 0x04,
    StringFormatSequenceModifer_Size        = 0x08,
    StringFormatSequenceModifer_Case        = 0x10,
} stringFormatSequenceModifier;

// typedef struct StringFormatToken_t {
//     Rune nextGlyph;
//     int reachedEndOfFormatString;
//     Error processingErrorOnLastPass;

//     Rune processingBuffer[16];

//     struct {
//         stringFormatSequenceModifier modifiersPresent;
//         Rune formatterType;     // 'l', 't', etc
//         unsigned int length;
//         Rune truncate;          // 'l' or 't'
//         Rune fillChar;
//         Rune fillCharDirection; // 'l' or 'r'
//         Rune size;              // one of the size modifiers
//         Rune xcase;              // 'l' or 'U'
//     } formatSequence;
// } StringFormatToken_t, *StringFormatToken;

// static unsigned int parseModifierFromFormatString( RuneString formatStringAfterModifierChar, StringFormatToken token )
// {
//     RuneString f = formatStringAfterModifierChar;
//     int i, j;
//     int64_t lengthModifierValue;

//     if (f[0] == 0 || f[1] == 0 || f[1] != U'=' || f[2] == 0)
//     {
//         token->processingErrorOnLastPass = ErrorInvalidFormat;
//         return 0;
//     }

//     switch (f[0])
//     {
//     case U'l':
//         token->formatSequence.modifiersPresent |= StringFormatSequenceModifer_Length;
//         for (j = 0, i = 2; j < 16 && f[i] != ',' && f[i] != '}'; j++, i++)
//         {
//             switch (f[i])
//             {
//             case U'0':
//             case U'1':
//             case U'2':
//             case U'3':
//             case U'4':
//             case U'5':
//             case U'6':
//             case U'7':
//             case U'8':
//             case U'9':
//                 token->processingBuffer[j] = f[i];
//                 break;

//             default:
//                 token->processingErrorOnLastPass = ErrorInvalidFormat;
//                 return 0;
//             }
//         }

//         if (j == 16) // number is too long
//         {
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//         token->processingBuffer[j] = 0;

//         Error e = DecimalIntegerAsStringToInt64( token->processingBuffer, j, &lengthModifierValue );
//         if (e != NoError)
//         {
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//         token->formatSequence.length = (unsigned int)lengthModifierValue;
//         return j - 1;

//     case U't':
//         token->formatSequence.modifiersPresent |= StringFormatSequenceModifer_Truncate;
//         switch (f[2])
//         {
//         case U'l':
//         case U'r':
//             token->formatSequence.truncate = f[2];
//             return 3;

//         default:
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//     case U'f':
//         token->formatSequence.modifiersPresent |= StringFormatSequenceModifer_Fill;
//         token->formatSequence.fillChar = f[2];

//         if (f[2])
//         {
//             switch (f[3])
//             {
//             case 0:
//             case U',':
//             case U'}':
//                 return 3;

//             case U'l':
//             case U'r':
//                 token->formatSequence.fillCharDirection = f[3];
//                 return 4;

//             default:
//                 token->processingErrorOnLastPass = ErrorInvalidFormat;
//                 return 0;
//             }
//         }
        
//         token->processingErrorOnLastPass = ErrorInvalidFormat;
//         return 0;

//     case U's':
//         token->formatSequence.modifiersPresent |= StringFormatSequenceModifer_Size;

//         switch (f[2])
//         {
//         case U'b':
//         case U'B':
//         case U's':
//         case U'S':
//         case U'i':
//         case U'I':
//         case U'q':
//         case U'Q':
//         case U'f':
//         case U'F':
//         case U'd':
//         case U'D':
//             token->formatSequence.size = f[2];
//             return 3;

//         default:
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//     case U'c':
//         token->formatSequence.modifiersPresent |= StringFormatSequenceModifer_Case;

//         switch (f[2])
//         {
//         case U'l':
//         case U'u':
//             token->formatSequence.xcase = f[2];
//             return 3;

//         default:
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//         break;

//     default:
//         token->processingErrorOnLastPass = ErrorInvalidFormat;
//         return 0;
//     }


// }

// static unsigned int processFormatSequenceInFormatString( RuneString formatStringAtStartOfFormatSequence, StringFormatToken token )
// {
//     if (*formatStringAtStartOfFormatSequence != U'{')
//     {
//         token->processingErrorOnLastPass = ErrorInvalidFormat;
//         return 0;
//     }

//     unsigned int glyphsConsumedFromFormatString = 1;
//     unsigned int lengthOfSequenceModifier;

//     RuneString f = formatStringAtStartOfFormatSequence + 1;

//     while (1) {
//         if ((lengthOfSequenceModifier = parseModifierFromFormatString( f, token )) == 0)
//             return 0;

//         f += lengthOfSequenceModifier;
//         glyphsConsumedFromFormatString += lengthOfSequenceModifier;

//         switch (*f)
//         {
//         case U'}':
//             return glyphsConsumedFromFormatString + 1;

//         case U',':
//             f += lengthOfSequenceModifier;
//             glyphsConsumedFromFormatString += lengthOfSequenceModifier;
//             break;

//         default:
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }
//     }
// }

// static unsigned int retrieveNextTokenFromFormatString( RuneString formatStringFromNextRune, StringFormatToken token )
// {
//     unsigned int runesConsumedFromFormatString = 0;
//     token->reachedEndOfFormatString = 0;
//     token->processingErrorOnLastPass = NoError;

//     switch (*formatStringFromNextRune)
//     {
//     case 0:
//         token->nextGlyph = 0;
//         token->reachedEndOfFormatString = 1;
//         return 1;

//     case U'%':
//         switch (formatStringFromNextRune[1])
//         {
//         case 0:
//             token->nextGlyph = U'%';
//             token->reachedEndOfFormatString = 1;
//             return 2;

//         case U'%':
//             token->nextGlyph = U'%';
//             return 2;

//         case U'r':
//         case U's':
//         case U'x':
//             return 2 + processFormatSequenceInFormatString( &formatStringFromNextRune[2], token );

//         default:
//             token->processingErrorOnLastPass = ErrorInvalidFormat;
//             return 0;
//         }

//     default:
//         token->nextGlyph = *formatStringFromNextRune;
//         return 1;
//     }

//     return runesConsumedFromFormatString;
// }

// static void iterativelyFormatIntoBufferUsingExplicitValist( RuneStringBuffer inBuffer, void (*OnNextFormatChunk)(RuneStringBuffer inBuffer, Error e, int amDone), void* extraArgs, const RuneString format, va_list varargs )
// {
//     if (inBuffer->Size < 2)
//         return OnNextFormatChunk( inBuffer, ErrorWouldCauseOverflow, 1 );

//     RuneString formatPtr = (RuneString)format;
//     unsigned int nextRuneOffsetToWriteInBuffer = 0;

//     while (nextRuneOffsetToWriteInBuffer < inBuffer->Size - 1)
//     {
//         switch (*formatPtr)
//         {
//         case 0:
//             inBuffer->String[nextRuneOffsetToWriteInBuffer] = 0;
//             OnNextFormatChunk(inBuffer, NoError, 1);
//             return;

//         case U'%':
//             break;

//         default:
//             inBuffer->String[nextRuneOffsetToWriteInBuffer++] = *formatPtr;
//             formatPtr++;
//         }
//     }

//     inBuffer->String[nextRuneOffsetToWriteInBuffer] = 0;

//     OnNextFormatChunk( inBuffer, NoError, 1 );
// }

// static void IterativelyFormatIntoBuffer(RuneStringBuffer inBuffer, void (*OnNextFormatChunk)( RuneStringBuffer inBuffer, Error e, int amDone, void* additionalArgs ), const RuneString format, ...)
// {
//     va_list varargs;
//     va_start( varargs, format );
//     iterativelyFormatIntoBufferUsingExplicitValist( inBuffer, OnNextFormatChunk, format, varargs );
//     va_end( varargs );
// }

// void formatterOnNextFormatChunkCallback( RuneStringBuffer inBuffer, Error e, int chunkerIsDone, void* stringFormatter )
// {

// }

// Error FormatIntoBuffer( StringFormatter f, RuneStringBuffer inBuffer, const RuneString format, ... )
// {
//     va_list varargs;
//     va_start( varargs, format );
//     iterativelyFormatIntoBufferUsingExplicitValist( inBuffer, OnNextFormatChunk, format, varargs );
//     va_end( varargs );
//     return NoError;
//}

static void formatIterativelyIntoBufferUsingExplicitVarags( RuneStringBuffer inBuffer, FormatBufferIteratingCallback callback, const RuneString format, va_list varargs )
{
    if (inBuffer->Size < 2)
    {
        callback->OnNextFormatChunk( inBuffer, ErrorWouldCauseOverflow, 1, callback->AdditionalArgs );
        return;
    }

    if (format == 0)
    {
        inBuffer->String[0] = 0;
        callback->OnNextFormatChunk( inBuffer, NoError, 1, callback->AdditionalArgs );
        return;
    }

    unsigned int glyphsWrittenToBufferForThisChunk = 0;
    RuneString fmt = (RuneString)format;


    while (*fmt)
    {
        switch (*fmt)
        {
        case U'%':
            break;

        default:
            inBuffer->String[glyphsWrittenToBufferForThisChunk++] = *fmt;
            if (glyphsWrittenToBufferForThisChunk >= inBuffer->Size - 1)
            {
                inBuffer->String[inBuffer->Size - 1] = 0;
                if (callback->OnNextFormatChunk( inBuffer, NoError, fmt[1] == 0 ? 1 : 0, callback->AdditionalArgs ))
                    return;
                glyphsWrittenToBufferForThisChunk = 0;
            }

            fmt++;
        }
    }

    inBuffer->String[glyphsWrittenToBufferForThisChunk] = 0;
    callback->OnNextFormatChunk( inBuffer, NoError, 1, callback->AdditionalArgs );
}

static void FormatIterativelyIntoBuffer( RuneStringBuffer inBuffer, FormatBufferIteratingCallback callback, const RuneString format, ... )
{
    va_list varargs;
    va_start( varargs, format );
    formatIterativelyIntoBufferUsingExplicitVarags( inBuffer, callback, format, varargs );
    va_end( varargs );
}

static int formatIntoBufferChunkCallback( RuneStringBuffer inBuffer, Error e, int done, void* additionalArgs )
{
    StringFormatter f = (StringFormatter)additionalArgs;

    if (e != NoError)
    {
        f->errorFromIterativeToFormat = e;
    }

    if (!done)
    {
        f->errorFromIterativeToFormat = ErrorWouldCauseOverflow;
        return 1;
    }

    f->errorFromIterativeToFormat = NoError;
    return 1;
}

static Error FormatIntoBuffer( StringFormatter f, RuneStringBuffer inBuffer, const RuneString format, ... )
{
    FormatBufferIteratingCallback_t callback = {
        .OnNextFormatChunk = formatIntoBufferChunkCallback,
        .AdditionalArgs = (void*)f,
    };

    va_list varargs;
    va_start(varargs, format);
    formatIterativelyIntoBufferUsingExplicitVarags( inBuffer, &callback, format, varargs );
    va_end(varargs);

    return NoError;
}


Error PopulateStringFormatter(StringFormatter f)
{
    f->FormatIterativelyIntoBuffer = FormatIterativelyIntoBuffer;
    f->FormatIntoBuffer = FormatIntoBuffer;
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
