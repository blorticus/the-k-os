#ifndef __APPS_KOSH__
#define __APPS_KOSH__

#include <video/text_terminal.h>


typedef struct {
    term_entry* term;
} kosh_shell;


void kosh_start_shell( kosh_shell* shell, term_entry* term );

#endif
