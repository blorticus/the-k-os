#include <stdio.h>
#include <sys/types.h>
#include <input/keyboard.h>

int getchar( void ) {
    int c;
    u16 scancodes;

    /* grab this right off of the kernel queue.  Obviously, not the right way, but since there is only one process right now... */
    for (;;)
        if ((c = read_next_key_stroke( &scancodes )))
            if ((char)c)    /* it is possible to just be a meta character, so we discard those */
                return (int)((char)c);  /* lower half is the character */
}
