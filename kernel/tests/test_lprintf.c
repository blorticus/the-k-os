#include <lcheck.h>
#include <stdio.h>
#include <sys/types.h>


char_t local_buf[100];
int bptr;

void reset_putchar_buf( void ) {
    int i;

    for (i = 0; i < 100; i++)
        local_buf[i] = 0;
    bptr = 0;
}

void t_putchar( void* v, int c ) {
    local_buf[bptr++] = (char_t)c;
}


#define local_printf(format, ...) lprintf(t_putchar, NULL, format, ## __VA_ARGS__)


int main( void ) {
    lcheck_suite* s;
    int c;

    s = create_suite( "lprintf" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"" );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"", 100 ) == 0,
                 "To lprintf, empty format string, no vararg params, buffer is empty" );
    fail_unless( s, c == 0,
                 "To lprintf, empty format string, no vararg params, count is zero" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L" abc\t123\nfoo bar" );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L" abc\t123\nfoo bar", 100 ) == 0,
                 "To lprintf, string but no formats, no vararg params, buffer is correct" );
    fail_unless( s, c == 16,
                 "To lprintf, string but no formats, no vararg params, count is 16" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%% abc\t%%123\nfoo%% bar%%" );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"% abc\t%123\nfoo% bar%", 100 ) == 0,
                 "To lprintf, string w/ percent literals only, no vararg params, buffer is correct" );
    fail_unless( s, c == 20,
                 "To lprintf, string w/ percent literals only, no vararg params, count is 20" );

    int p = 0;
    unsigned short x = 0xab34;
    //const char_t* str = "foo bar\"'\n";
    const char* str = "foo bar";

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%d", (long)123 );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"123", 100 ) == 0,
                 "To lprintf, single %d, one constant, buffer is correct" );
    fail_unless( s, c == 3,
                 "To lprintf, single %d, one constant, count is correct" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%d", -345690 );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"-345690", 100 ) == 0,
                 "To lprintf, single %d, one constant, buffer is correct" );
    fail_unless( s, c == 7,
                 "To lprintf, single %d, one constant, count is correct" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%i", p );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"0", 100 ) == 0,
                 "To lprintf, single %i, one var (value 0), buffer is correct" );
    fail_unless( s, c == 1,
                 "To lprintf, single %d, one var (value 0), count is correct" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%x", x );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"ab34", 100 ) == 0,
                 "To lprintf, single %x, one var (value ab34), buffer is correct" );
    fail_unless( s, c == 4,
                 "To lprintf, single %x, one var (value ab34), count is correct" );

    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%x", (int)-3456789 );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"ffcb40eb", 100 ) == 0,
                 "To lprintf, single %x, one value (value -3456789), buffer is correct" );
    fail_unless( s, c == 8,
                 "To lprintf, single %x, one value (value -3456789), count is correct" );


    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"%s", "this is the %% time\n" );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"this is the %% time\n", 100 ) == 0,
                 "To lprintf, single %s, literal value, buffer is correct" );
    fail_unless( s, c == 20,
                 "To lprintf, single %s, literal value, count is correct" );


    reset_putchar_buf();
    c = lprintf( t_putchar, NULL, L"1a%%23%i-%x%i,%s%d<>%%%x%s%c\n", -234, x, p, "this", 1576342, 0x0, str, 'a' );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"1a%23-234-ab340,this1576342<>%0foo bara\n", 100 ) == 0,
                 "To lprintf, string w/ percent expansions for %, i, x, d, s, c, varargs w/ vars, buffer is correct" );
    fail_unless( s, c == 40,
                 "To lprintf, string w/ percent expansions for %, i, x, d, s, c, count is 40" );

    reset_putchar_buf();
    c = local_printf( L"1a%%23%i-%x%i,%s%d<>%%%x%s%c\n", -234, x, p, "this", 1576342, 0x0, str, 'a' );

    fail_unless( s, __ustrncmp( (const char_t*)local_buf, L"1a%23-234-ab340,this1576342<>%0foo bara\n", 100 ) == 0,
                 "To lprintf, string w/ percent expansions for %, i, x, d, s, c, varargs w/ vars, using macro, buffer is correct" );
    fail_unless( s, c == 40,
                 "To lprintf, string w/ percent expansions for %, i, x, d, s, c, using macro, count is 40" );


    return conclude_suite( s );
}

