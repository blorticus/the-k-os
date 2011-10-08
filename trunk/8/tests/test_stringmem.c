#include <string.h>
#include <sys/types.h>
#include <lcheck.h>

u8 mem8[100];

void clear_mem8() {
    int i;
    for (i = 0; i < 100; i++)
        mem8[i] = '\0';
}

int main( void ) {
    lcheck_suite* s;
    int i;
    int ok;

    void* b;

    clear_mem8();

    s = create_suite( "stringmem" );

    /* TEST 1: memset */
    b = memset( mem8, '-', 30 );

    fail_unless( s, __strncmp( (char*)mem8, "------------------------------", 40 ) == 0,
                 "TEST 1: memset() to 30 dashes does not produce 30 dashes" );

    fail_unless( s, b == mem8,
                 "TEST !: memset() to 30 dashes does not return original buffer pointer" );

    memset( mem8, '\0', 30 );

    ok = 1;
    for (i = 0; i < 30; i++)
        if (mem8[i] != 0)
            ok = 0;

    fail_unless( s, ok,
                 "TEST 1: memset() to 30 NULLs does not produce 30 NULLs" );

    clear_mem8();

    return conclude_suite( s );
}
