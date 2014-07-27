#include <lcheck.h>
#include <stdio.h>

//#define NULL ((char*)'\0')

extern int cprintf( void (*putchar_f)(int), const char *fmt, ... );

char local_buf[100];
int bptr;

void reset_putchar_buf( void ) {
    int i;

    for (i = 0; i < 100; i++)
        local_buf[i] = '\0';
    bptr = 0;
}

void t_putchar( int c ) {
    local_buf[bptr++] = (char)c;
}


#define local_printf(format, ...) cprintf(t_putchar, NULL, format, ## __VA_ARGS__)


int main( void ) {
    lcheck_suite* s;
    int c;

    s = create_suite( "cprintf" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "" );

    fail_unless( s, __strncmp( (const char*)local_buf, "", 100 ) == 0,
                 "To cprintf, empty format string, no vararg params, buffer is empty" );
    fail_unless( s, c == 0,
                 "To cprintf, empty format string, no vararg params, count is zero" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, " abc\t123\nfoo bar" );

    fail_unless( s, __strncmp( (const char*)local_buf, " abc\t123\nfoo bar", 100 ) == 0,
                 "To cprintf, string but no formats, no vararg params, buffer is correct" );
    fail_unless( s, c == 16,
                 "To cprintf, string but no formats, no vararg params, count is 16" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%% abc\t%%123\nfoo%% bar%%" );

    fail_unless( s, __strncmp( (const char*)local_buf, "% abc\t%123\nfoo% bar%", 100 ) == 0,
                 "To cprintf, string w/ percent literals only, no vararg params, buffer is correct" );
    fail_unless( s, c == 20,
                 "To cprintf, string w/ percent literals only, no vararg params, count is 20" );

    int p = 0;
    unsigned short x = 0xab34;
    //const char* str = "foo bar\"'\n";
    const char* str = "foo bar";

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%d", (long)123 );

    fail_unless( s, __strncmp( (const char*)local_buf, "123", 100 ) == 0,
                 "To cprintf, single %d, one constant, buffer is correct" );
    fail_unless( s, c == 3,
                 "To cprintf, single %d, one constant, count is correct" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%d", -345690 );

    fail_unless( s, __strncmp( (const char*)local_buf, "-345690", 100 ) == 0,
                 "To cprintf, single %d, one constant, buffer is correct" );
    fail_unless( s, c == 7,
                 "To cprintf, single %d, one constant, count is correct" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%i", p );

    fail_unless( s, __strncmp( (const char*)local_buf, "0", 100 ) == 0,
                 "To cprintf, single %i, one var (value 0), buffer is correct" );
    fail_unless( s, c == 1,
                 "To cprintf, single %d, one var (value 0), count is correct" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%x", x );

    fail_unless( s, __strncmp( (const char*)local_buf, "ab34", 100 ) == 0,
                 "To cprintf, single %x, one var (value ab34), buffer is correct" );
    fail_unless( s, c == 4,
                 "To cprintf, single %x, one var (value ab34), count is correct" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%x", (long)-3456789 );

    fail_unless( s, __strncmp( (const char*)local_buf, "ffcb40eb", 100 ) == 0,
                 "To cprintf, single %x, one value (value -3456789), buffer is correct" );
    fail_unless( s, c == 8,
                 "To cprintf, single %x, one value (value -3456789), count is correct" );


    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "%s", "this is the %% time\n" );

    fail_unless( s, __strncmp( (const char*)local_buf, "this is the %% time\n", 100 ) == 0,
                 "To cprintf, single %s, literal value, buffer is correct" );
    fail_unless( s, c == 20,
                 "To cprintf, single %s, literal value, count is correct" );


    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "1a%%23%i-%x%i,%s\n", -234, x, p, str );
    fail_unless( s, __strncmp( (const char*)local_buf, "1a%23-234-ab340,foo bar\n", 100 ) == 0,
                 "To cprintf, string w/ percent expansions for %, i, x, s, varargs w/ vars, buffer is correct" );

    reset_putchar_buf();
    c = cprintf( t_putchar, NULL, "1a%%23%i-%x%i,%s%d<>%%%x%s%c\n", -234, x, p, "this", 1576342, 0x0, str, 'a' );

    fail_unless( s, __strncmp( (const char*)local_buf, "1a%23-234-ab340,this1576342<>%0foo bara\n", 100 ) == 0,
                 "To cprintf, string w/ percent expansions for %, i, x, d, s, c, varargs w/ vars, buffer is correct" );
    fail_unless( s, c == 40,
                 "To cprintf, string w/ percent expansions for %, i, x, d, s, c, count is 40" );

    reset_putchar_buf();
    c = local_printf( "1a%%23%i-%x%i,%s%d<>%%%x%s%c\n", -234, x, p, "this", 1576342, 0x0, str, 'a' );

    fail_unless( s, __strncmp( (const char*)local_buf, "1a%23-234-ab340,this1576342<>%0foo bara\n", 100 ) == 0,
                 "To cprintf, string w/ percent expansions for %, i, x, d, s, c, varargs w/ vars, using macro, buffer is correct" );
    fail_unless( s, c == 40,
                 "To cprintf, string w/ percent expansions for %, i, x, d, s, c, using macro, count is 40" );


    return conclude_suite( s );
}

