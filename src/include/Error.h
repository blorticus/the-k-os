#pragma once

typedef enum Error_t {
    NoError = 0,
    ErrorInvalidParameter,
    ErrorValueUndefined,
    ErrorWouldCauseOverflow,
    ErrorInvalidFormat,
    ErrorNotANumber,
    ErrorNullValue,
    ErrorBufferTooSmall,
    ErrorFacilityNotPresent,
    ErrorVersionNotImplemented,
    ErrorIncorrectChecksum,
} Error;