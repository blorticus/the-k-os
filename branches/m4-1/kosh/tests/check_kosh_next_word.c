#include <lcheck.h>
#include <kosh/koshlib.h>
#include <string.h>

int main( void ) {
    lcheck_suite* s;

    s = create_suite( "kosh_next_word" );

    char n[100] = { 'w', 'r', 'o', 'n', 'g', '\0' };

    next_word( "", n, 99 );

    fail_unless( s, strcmp( n, "" ) == 0,
                 "null string fed to next_word() does not return null string" );

    next_word( "this", n, 99 );

    fail_unless( s, strcmp( n, "this" ) == 0,
                 "single word fed to next_word() does not return that word" );

    next_word( "that is", n, 99 );

    fail_unless( s, strcmp( n, "that" ) == 0,
                 "two words with single space fed to next_word() does not return first word" );

    next_word( "here ", n, 99 );

    fail_unless( s, strcmp( n, "here" ) == 0,
                 "one word with single trailing space fed to next_word() does not return first word" );

    next_word( "where\tis", n, 99 );

    fail_unless( s, strcmp( n, "where" ) == 0,
                 "two words with tab fed to next_word() does not return first word" );

    next_word( "yoodle\t  is", n, 99 );

    fail_unless( s, strcmp( n, "yoodle" ) == 0,
                 "two words with tab and spaces fed to next_word() does not return first word" );

    next_word( "porkisintheair\t \t is", n, 99 );

    fail_unless( s, strcmp( n, "porkisintheair" ) == 0,
                 "two words with tabs and spaces fed to next_word() does not return first word" );

    next_word( "my\t \t is\t", n, 99 );

    fail_unless( s, strcmp( n, "my" ) == 0,
                 "two words with tabs, spaces and trailing tab fed to next_word() does not return first word" );

    next_word( "this\t \n is\t\n", n, 99 );

    fail_unless( s, strcmp( n, "this" ) == 0,
                 "two words with tabs, spaces, newline and trailing tab and newline fed to next_word() does not return first word" );

    next_word( "   e", n, 99 );

    fail_unless( s, strcmp( n, "e" ) == 0,
                 "one word with leading spaces fed to next_word() does not return first word" );

    next_word( "   this   is", n, 99 );

    fail_unless( s, strcmp( n, "this" ) == 0,
                 "two words with leading spaces and multiple inter-word spaces fed to next_word() does not return first word" );

    next_word( "   ", n, 99 );

    fail_unless( s, strcmp( n, "" ) == 0,
                 "string with only spaces fed to new_word() does not return null" );

    next_word( "\t", n, 99 );

    fail_unless( s, strcmp( n, "" ) == 0,
                 "string with only tab fed to new_word() does not return null" );

    /* now test for multiple word iteration */
    next_word( " this\tis\na \t simple sentence.  \n  \t ", n, 99 );

    fail_unless( s, strcmp( n, "this" ) == 0,
                 "first call to next_word() w/ multiple words does not return first word" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "is" ) == 0,
                 "second call to next_word() w/ multiple words does not return second word" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "a" ) == 0,
                 "third call to next_word() w/ multiple words does not return third word" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "simple" ) == 0,
                 "fourth call to next_word() w/ multiple words does not return fourth word" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "sentence." ) == 0,
                 "fifth call to next_word() w/ multiple words does not return fifth word" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "" ) == 0,
                 "first call to next_word() after consuming all word tokens does not return empty string" );

    next_word( NULL, n, 99 );

    fail_unless( s, strcmp( n, "" ) == 0,
                 "second call to next_word() after consuming all word tokens does not return empty string" );

    return conclude_suite( s );
}
