#include <kerror.h>


term_entry* ERROR_TERMINAL = '\0';


void kerror_init( term_entry* error_terminal ) {
    ERROR_TERMINAL = error_terminal;
}


void kerror( const wchar_t* str ) {
    term_set_pos( ERROR_TERMINAL, 0, 0 );
    term_printf( "%ls\n", str );
}
