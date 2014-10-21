#ifndef __APPS_KOSH__
#define __APPS_KOSH__

#include <video/text_terminal.h>
#include <apps/kosh/kosh_cmd.h>


typedef struct {
    term_entry* term;
    kosh_cmd_list cmd_list;
} kosh_shell;


void kosh_start_shell( kosh_shell* shell, term_entry* term );

#endif
