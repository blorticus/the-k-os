#ifndef DEBUGGING
#define DEBUGGING

/**
 *
 * DESCRIPTION:         Contains symbols that can be used to help with debugging.  Symbols are
 *                      only active if DEBUG is set for compilation
 *
 **/

#ifdef DEBUG

#include <video/kterm.h>

void set_debug_kterm_window( KTERM_WINDOW w );
KTERM_WINDOW get_debug_kterm_window();

#endif  // DEBUG

#endif
