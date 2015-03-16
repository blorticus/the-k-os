#ifndef __KERROR__
#define __KERROR__

#include <sys/types.h>
#include <video/text_terminal.h>

void kerror_init( term_entry* error_terminal );
void kerror( const wchar_t* str, ... );
void kerror_silly( u8 interrupt, u8 irq );

#endif
