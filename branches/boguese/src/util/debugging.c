#ifdef DEBUG

#include <util/debugging.h>

KTERM_WINDOW debug_win;
void set_debug_kterm_window( KTERM_WINDOW w ) {
    debug_win = w;
}


KTERM_WINDOW get_debug_kterm_window() {
    return debug_win;
}


#endif
