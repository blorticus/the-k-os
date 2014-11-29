#ifndef __STDIO_H__
#define __STDIO_H__

#include <sys/types.h>

/**
 * BRIEF:           A (weak) attempt at implementing C stdlib's stdio
 * BACKGROUND:      C standard I/O routines
 * SEE:             -
 * RE-ENTRANT?:     PER-METHOD
 ***/


/**
 *
 * DESCRIPTION:     Supposed to grab next character from stdin, but since there's no concept of file I/O at this point, simply grab the next key
 *                  in the keyboard buffer.  Block (uninterruptably) until at least one key is pressed.
 * RETURN:          the ASCII character pressed (possibly shifted)
 * SIDE-EFFECTS:    -
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
int getchar( void );


/**
 *
 * DESCRIPTION:     A generalized printf() routine.  'putchar_f' is a pointer to a function that handles each character produced by this routine;
 *                  (notice that putchar_f takes an int (the ascii character produced by this method) and varargs) 'putchar_args' is a pointer to
 *                  additional arguments expected by 'putchar_f'; 'fmt' is a NULL terminated printf format string; and varargs are the filler values
 *                  for % substitutions in 'fmt'
 * RETURN:          The number of characters pushed to 'putchar_f'
 * SIDE-EFFECTS:    -
 * NOTES:           No check is made that varargs has enough arguments for 'fmt'.  If there is a mismatch, the results are undefined.
 * RE-ENTRANT?:     NO
 *
 */ 
int cprintf( void (*putchar_f)(int, ...), void (*putwchar_f)(wchar_t, ...), char* putchar_args, const char *fmt, ... );


#endif
