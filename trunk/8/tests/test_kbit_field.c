#include <lcheck.h>
#include <util/kbit_field.h>
#include <sys/types.h>

char buf[100];

static void test_bit_range_for_clear( lcheck_suite* s, kbit_field *kb, u32 first_bit, u32 last_bit, char *test_descriptor ) {
    int i;

    for (i = first_bit; i <= last_bit; i++) {
        __sprintf( buf, "bit field %d is set %s but should not be", i, test_descriptor );
        fail_unless( s, !kbit_field_is_set( kb, i ), buf );
    }
}

int main( void ) {
    lcheck_suite* s;

    kbit_field kb;
    u32 fields[5];

    s = create_suite( "kbit_field" );

    kbit_field_init( &kb, fields, 140 );

    test_bit_range_for_clear( s, &kb, 0, 139, "after init" );

    kbit_field_set( &kb, 10 );

    test_bit_range_for_clear( s, &kb, 0, 9, "when only bit 10 was set" );

    fail_unless( s, kbit_field_is_set( &kb, 10 ), "bit field 10 is not set but should be after only 10 set" );

    test_bit_range_for_clear( s, &kb, 11, 139, "when only bit 10 was set" );

    kbit_field_clear( &kb, 10 );

    fail_unless( s, !kbit_field_is_set( &kb, 10 ), "bit field 10 is set but should not be after a clear" );

    kbit_field_set( &kb, 0 );
    kbit_field_set( &kb, 10 );
    kbit_field_set( &kb, 11 );
    kbit_field_set( &kb, 12 );
    kbit_field_set( &kb, 50 );
    kbit_field_set( &kb, 100 );
    kbit_field_set( &kb, 101 );
    kbit_field_set( &kb, 130 );
    kbit_field_set( &kb, 139 );

    fail_unless( s, kbit_field_is_set( &kb, 0 ), "bit field 0 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 1, 9, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 10 ), "bit field 10 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 11 ), "bit field 11 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 12 ), "bit field 12 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 13, 49, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 50 ), "bit field 50 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 51, 99, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 100 ), "bit field 100 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 101 ), "bit field 101 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 102, 129, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 130 ), "bit field 130 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 131, 138, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 139 ), "bit field 139 not set but should be after group set" );

    kbit_field_clear( &kb, 0 );

    test_bit_range_for_clear( s, &kb, 0, 9, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 10 ), "bit field 10 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 11 ), "bit field 11 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 12 ), "bit field 12 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 13, 49, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 50 ), "bit field 50 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 51, 99, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 100 ), "bit field 100 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 101 ), "bit field 101 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 102, 129, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 130 ), "bit field 130 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 131, 138, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 139 ), "bit field 139 not set but should be after group set" );

    kbit_field_clear( &kb, 130 );

    test_bit_range_for_clear( s, &kb, 0, 9, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 10 ), "bit field 10 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 11 ), "bit field 11 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 12 ), "bit field 12 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 13, 49, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 50 ), "bit field 50 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 51, 99, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 100 ), "bit field 100 not set but should be after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 101 ), "bit field 101 not set but should be after group set" );
    test_bit_range_for_clear( s, &kb, 102, 138, "after group set" );
    fail_unless( s, kbit_field_is_set( &kb, 139 ), "bit field 139 not set but should be after group set" );


    return conclude_suite( s );
}


