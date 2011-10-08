#include <lcheck.h>
#include <string.h>
#include <sys/types.h>

char* fail_strings[] = { "foo", "x800", "80x0", "" };
char* succeed_strings[] = { "0", "-0", "+0", "100", "-100", "102435", "+102435", "-345678" };
char testname[100];

int main( void ) {
    lcheck_suite* s;
    int i;

    s = create_suite( "isstringint" );

    for (i = 0; i < ((unsigned int)(sizeof( fail_strings )) / 4); i++) {
        __sprintf( testname, "string should fail: %s", fail_strings[i] );
        fail_unless( s, !isstringint( fail_strings[i] ), testname );
    }

    for (i = 0; i < ((unsigned int)(sizeof( succeed_strings )) / 4); i++) {
        __sprintf( testname, "string should fail: %s", succeed_strings[i] );
        fail_unless( s, isstringint( succeed_strings[i] ), testname );
    }


    return conclude_suite( s );
}

