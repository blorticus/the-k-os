#include <lcheck.h>
#include <string.h>
#include <sys/types.h>

// first list is a string,
// second list is base
// third list is long it should produce,
// fourth list is what endptr should point to if 'endptr' is not null.
//   This is an offset into strings (so if strings[5] is "21f34", endptr[5] should be 2 -- the number of characters into strings[5] that it points)
// All lists must have same number of members.
const char* strings[] = { "0", "+0", "-0", "0x0", "0x000", "00", "+00", "-00", "-0x00", "+0x0000", "10", "10", "10", "1f", "1f", "1f", "fb34foo", "fb34foo", "fb34foo", "-fb34foo", "!034", "!034", "!034", "4bEEf", "-0x4bEEf", "zy4", "14531", "-14531"   };
const int   base[]    = { 0,   0,    0,    0,     0,       0,    0,     0,     0,       0,         10,   8,    16,   10,   8,    16,   10,        8,         16,        16,         10,     8,      16,     16,      16,         36,    10,      10         };
const long  value[]   = { 0,   0,    0,    0,     0,       0,    0,     0,     0,       0,         10,   8,    16,   1,    1,    31,   0,         0,         1028943,   -1028943,   0,      0,      0,      311023,  -311023,    46588, 14531,   -14531     };
int  endptr_offset[]  = { 1,   2,    2,    3,     5,       2,    3,     3,     5,       7,         2,    2,    2,    1,    1,    2,    0,         0,         5,         6,          0,      0,      0,      5,       8,          3,     5,       6          };

int main( void ) {
    lcheck_suite* s;
    int testset;
    char testname[125];
    char* endptr;
    long retval;

    s = create_suite( "strtol" );

    for (testset = 0; testset < sizeof( strings ) / 4; testset++) {
        __sprintf( testname, "value incorrect for string [%s] base [%i] with endptr (testset == %d)", strings[testset], base[testset], testset );
        retval = strtol( strings[testset], &endptr, base[testset] );
        fail_unless( s, retval == value[testset], testname );
        __sprintf( testname, "endptr incorrect for string [%s] base [%i] with endptr (testset == %d)", strings[testset], base[testset], testset );
        fail_unless( s, endptr == strings[testset] + endptr_offset[testset], testname );

        __sprintf( testname, "value incorrect for string [%s] base [%i] without endptr (testset == %d)", strings[testset], base[testset] );
        endptr = NULL;
        retval = strtol( strings[testset], NULL, base[testset] );
        fail_unless( s, retval == value[testset], testname );
        __sprintf( testname, "endptr incorrect for string [%s] base [%i] without endptr (testset == %d)", strings[testset], base[testset] );
        fail_unless( s, endptr == NULL, testname );
    }

    return conclude_suite( s );
}

