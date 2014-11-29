#ifndef __KERROR__
#define __KERROR__

#include <sys/types.h>
#include <video/text_terminal.h>

void kerror_init( term_entry* error_terminal );
void kerror( wchar_t* str );

#endif
