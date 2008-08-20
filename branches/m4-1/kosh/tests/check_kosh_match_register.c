#include <check.h>
#include <sys/types.h>
#include <kosh/koshlib.h>


START_TEST (testing_match_register)
{
    fail_unless( match_register( "" ) == -1,
                 "match_register() on null string does not return expected result" );

    /* test of all possibilities with varying case */
    fail_unless( match_register( "al" ) == AL,
                 "match_register for 'al' does not return AL" );
    fail_unless( match_register( "AH" ) == AH,
                 "match_register for 'AH' does not return AH" );
    fail_unless( match_register( "Bl" ) == BL,
                 "match_register for 'Bl' does not return BL" );
    fail_unless( match_register( "bH" ) == BH,
                 "match_register for 'bH' does not return BH" );
}
END_TEST


Suite* suite( void ) {
    Suite* s = suite_create( "match_register" );

    TCase *tc_core = tcase_create( "Core" );
    tcase_add_test( tc_core, testing_match_register );
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
