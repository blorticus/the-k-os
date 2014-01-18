#include <lcheck.h>
#include <YOURLIBRARY.h>
#include <sys/types.h>

int main( void ) {
    lcheck_suite* s;

    s = create_suite( "YOUR_MODULE" );

    fail_unless( s, 1 == 0, "A SAMPLE TEST" );

    return conclude_suite( s );
}

