#ifndef __VIDEO_FONT__
#define __VIDEO_FONT__

#include <sys/types.h>

typedef enum {
    ASCII_8x16 = 1,
} BUILT_IN_FONTS;

// return null if font is not defined
void* font_get_definition( BUILT_IN_FONTS font );


#endif
