#include <check.h>
#include <sys/types.h>

/* This tests methods internal to kosh.c so those methods aren't in a header file */
extern char* next_word( char* str, char* buffer, unsigned int limit ); //_U32 limit );


START_TEST (testing_next_word)
{
    char n[100] = { 'w', 'r', 'o', 'n', 'g', '\0' };

    next_word( "", n, 99 );

    fail_unless( strcmp( n, "" ) == 0,
                 "null string fed to next_word() does not return null string" );

    next_word( "this", n, 99 );

    fail_unless( strcmp( n, "this" ) == 0,
                 "single word fed to next_word() does not return that word" );

    next_word( "that is", n, 99 );

    fail_unless( strcmp( n, "that" ) == 0,
                 "two words with single space fed to next_word() does not return first word" );

    next_word( "here ", n, 99 );

    fail_unless( strcmp( n, "here" ) == 0,
                 "one word with single trailing space fed to next_word() does not return first word" );

    next_word( "where\tis", n, 99 );

    fail_unless( strcmp( n, "where" ) == 0,
                 "two words with tab fed to next_word() does not return first word" );

    next_word( "yoodle\t  is", n, 99 );

    fail_unless( strcmp( n, "yoodle" ) == 0,
                 "two words with tab and spaces fed to next_word() does not return first word" );

    next_word( "porkisintheair\t \t is", n, 99 );

    fail_unless( strcmp( n, "porkisintheair" ) == 0,
                 "two words with tabs and spaces fed to next_word() does not return first word" );

    next_word( "my\t \t is\t", n, 99 );

    fail_unless( strcmp( n, "my" ) == 0,
                 "two words with tabs, spaces and trailing tab fed to next_word() does not return first word" );

    next_word( "this\t \n is\t\n", n, 99 );

    fail_unless( strcmp( n, "this" ) == 0,
                 "two words with tabs, spaces, newline and trailing tab and newline fed to next_word() does not return first word" );

    next_word( "   e", n, 99 );

    fail_unless( strcmp( n, "e" ) == 0,
                 "one word with leading spaces fed to next_word() does not return first word" );

    next_word( "   this   is", n, 99 );

    fail_unless( strcmp( n, "this" ) == 0,
                 "two words with leading spaces and multiple inter-word spaces fed to next_word() does not return first word" );

    next_word( "   ", n, 99 );

    fail_unless( strcmp( n, "" ) == 0,
                 "string with only spaces fed to new_word() does not return null" );

    next_word( "\t", n, 99 );

    fail_unless( strcmp( n, "" ) == 0,
                 "string with only tab fed to new_word() does not return null" );
}
END_TEST


Suite* suite( void ) {
    Suite* s = suite_create( "next_word" );

    TCase *tc_core = tcase_create( "Core" );
    tcase_add_test( tc_core, testing_next_word );
    suite_add_tcase( s, tc_core );

    return s;
}


int main( void ) {
    int number_failed;
    Suite* s = suite();
    SRunner* sr = srunner_create( s );
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed( sr );
    srunner_free( sr );
    return number_failed;
}
