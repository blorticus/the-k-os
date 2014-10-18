#include <lcheck.h>
#include <string.h>
#include <sys/types.h>

int main( void ) {
    lcheck_suite* s;

    s = create_suite( "strncmp" );

    fail_unless( s, strncmp( "", "", 10 ) == 0, "Two empty strings are equal" );
    fail_unless( s, strncmp( "abc", "abcd", 5 ) < 0, "strcnmp( 'abc', 'abcd', 5 ) < 0" );
    fail_unless( s, strncmp( "abc", "abcd", 3 ) == 0, "strcnmp( 'abc', 'abcd', 3 ) == 0" );
    fail_unless( s, strncmp( "Abc", "abcd", 5 ) < 0, "strcnmp( 'Abc', 'abcd', 5 ) < 0" );
    fail_unless( s, strncmp( "abc", "Abcd", 5 ) > 0, "strcnmp( 'abc', 'Abcd', 5 ) > 0" );

    return conclude_suite( s );
}

