#include <string.h>
#include <lcheck.h>


#define MAX_STRING_LENGTH   150

void do_test( lcheck_suite* s, char* string, const char* expected_string, unsigned int expected_count, char* testname ) {
    int count;
    int r;

    char* ns = __strcat_realloc( "", string );  /* if string is really a const char, we need to allocate a buffer for it */

    count = strntolower( ns, MAX_STRING_LENGTH );

    r = __strncmp( ns, expected_string, MAX_STRING_LENGTH );

    fail_unless( s, r == 0,
                 __strcat_realloc( "strntolower() string compare failes on test ", testname ) );

    fail_unless( s, count == expected_count,
                 __strcat_realloc( "strntolower() string count fails on test ", testname ) );
}


int main( void ) {
    lcheck_suite* s;

    /* all ascii chars, then all ascii chars with capital letters converted to lower-case */
    char n[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
                 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
                 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, '\0' };
    char m[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
                 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
                 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
                 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, '\0' };

    s = create_suite( "strntolower" );

    do_test( s, "", "", 0, "empty string" );
    do_test( s, "  ", "  ", 0, "only whitespace" );
    do_test( s, "a", "a", 0, "one lowercase letter" );
    do_test( s, "A", "a", 1, "one uppercase letter" );
    do_test( s, "bZ", "bz", 1, "one lower- and one uppercase letter" );
    do_test( s, "ZXCVBNMASDFGHJKLQWERTYUIOP", "zxcvbnmasdfghjklqwertyuiop", 26, "all uppercase letters" );
    do_test( s, "zxcvbnmasdfghjklqwertyuiop", "zxcvbnmasdfghjklqwertyuiop", 0, "all lowercase letters" );
    do_test( s, "ZzXxCcVvBbNnMmAaSsDdFfGgHhJjKkLlQqWwEeRrTtYyUuIiOoPp", "zzxxccvvbbnnmmaassddffgghhjjkkllqqwweerrttyyuuiioopp", 26, "all letters, one cap, one lower" );
    do_test( s, "The QUICK brown FoX jumPS OVEr tHe lAzY dog", "the quick brown fox jumps over the lazy dog", 16, "The quick brown fox... mixed case" );
    do_test( s, (char*)&n, (char*)&m, 26, "all asci chars 1 .. 127" );

    return conclude_suite( s );
}
