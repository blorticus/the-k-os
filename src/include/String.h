#pragma once

#include <Error.h>
#include <stdint.h>
#include <stdarg.h>

typedef uint32_t Rune;
typedef Rune *RuneString;

typedef struct RuneStringBuffer_t {
    RuneString String;
    unsigned int Size;
} RuneStringBuffer_t, *RuneStringBuffer;


// FormattedString formats look like this:
//   U"The register is (0x%x{l=10,f=0,s=q,c=U}), string = %r{l=10,t=l,f= l}", regValue, someString
//   U"The register is (%{08x}), %{-30.30 r}", regValue, someString
// The general use is %<type>, where <type> is always a single letter.  It is optionally
// followed by {<formatters>}, where <formatters> are a comma-delimited list of format options.
// The literal pair %% is interpretted as %.  As such, %%x{ would become the literal %x{.
// A %<type> for a letter that is not known truncates the output string at the point immediately
// before the % and ErrorInvalidFormat is returned.
//
// The <type>s are:
//      x   hex
//      d   decimal
//      r   RuneString
//
// The <formatters> are:
//      l=<n>       the length, which is potentially modified by t= or f=.  If t= is not provided,
//                  this is interpretted as a minimum length.  If the generated value is shorter
//                  than this number of Runes, the resulting substitution is padded to the left
//                  with the f= fill (which is assumed to be {f= } if f= isn't given);
//      t=l|r       if present, then l= becomes a maximum length.  If the generated value is longer
//                  than this number of Runes, the resulting substitution is truncated.  If
//                  t=l, it is truncated from the left (e.g., if the string is "abcdefg", l=4 and t=l,
//                  the resulting substitution is "abcd").  If t=r, it is truncated from the right
//                  (e.g., if the string is "abcdefg", l=4 and t=r, the resulting substitution is "defg");
//      f=<c>[l|r]  the fill, where <c> is a single Rune.  If l= is given and f= is given, then
//                  l= is a minimum length (it may also be a maximum length if t= is given).  The resulting
//                  substitution must have at least l= Runes.  If the antecedent l is provided, then the
//                  substitution is filled from the left (e.g., if the string is "abc", l=5 and f=0l, then
//                  the resulting string is 00abc).  If the antecedent r is provided, then the substitution
//                  is filled from the right.  If neither is given, l is assumed.
//      s=<t>       interpretted numberic value.  <t> may be one from the set [bBsSiIqQfFdD]
//                  uint8_t (b), int8_t (B), uint16_t (s), int16_t (S), uint32_t (i), int32_t (I),
//                  uint64_t (q), int64_t (Q), ufloat32_t (f), float32_t (F), ufloat64_t (d), float64_t (D).
//                  The signed vs. unsigned nature only affects the <type> d.  These values can only be used with <type> d or x.
//                  <t>=f|F|d|D only makes sense.  Everything else is an error (meaning the output string is truncated before
//                  this substitution and ErrorInvalidFormat is returned).
//      c=l|U       case of hex values, either "l"ower or "U"pper.  Can only be used with <type> x.  Everything else is an
//                  error.
//
// %bx, %sx, %ix, %qx, %0bx, %0sx, %0ix, %0qx
// %bd, %sd, %id, %qd
// %if, %qf
// %r
//

typedef struct FormatBufferIteratingCallback_t {
    int (*OnNextFormatChunk)( RuneStringBuffer inBuffer, Error e, int done, void* additionalArgs ); // return non-zero to terminate processing
    void* AdditionalArgs;
} FormatBufferIteratingCallback_t, *FormatBufferIteratingCallback;


typedef struct StringFormatter_t {
    Error errorFromIterativeToFormat;

    void (*FormatIterativelyIntoBuffer)( RuneStringBuffer inBuffer, FormatBufferIteratingCallback, const RuneString format, ... );
    Error (*FormatIntoBuffer)( struct StringFormatter_t* f, RuneStringBuffer inBuffer, const RuneString format, ... );
    Error (*Uint64ToDecimalString)( RuneStringBuffer inBuffer, uint64_t v );
    Error (*Int64ToDecimalString)( RuneStringBuffer inBuffer, int64_t v );
    Error (*Uint64ToHexString)( RuneStringBuffer inBuffer, uint64_t v );
    Error (*Uint64ToZeroPaddedHexString)( RuneStringBuffer inBuffer, uint64_t v, unsigned int outputFieldLength );
} *StringFormatter;

Error PopulateStringFormatter( StringFormatter f );

Error DecimalIntegerAsStringToInt64( const RuneString buffer, unsigned int maximumBufferLength, int64_t *value );
Error Uint64ToDecimalString( RuneStringBuffer inBuffer, uint64_t v );
Error Int64ToDecimalString( RuneStringBuffer inBuffer, int64_t v );
Error Uint64ToHexString( RuneStringBuffer inBuffer, uint64_t v );
Error Uint64ToZeroPaddedHexString( RuneStringBuffer inBuffer, uint64_t v, unsigned int outputFieldLength );
