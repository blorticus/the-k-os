#include <lcheck.h>
#include <input/kb-ps2.h>
#include <sys/types.h>

int main( void ) {
    lcheck_suite* s;
    ScHwQueue sc1;
    u8* queue_slots = (u8*)__malloc( sizeof( u8 ) * 4 );

    s = create_suite( "sc_hw_queue_kb_ps2" );

    hw_sc_queue_init( &sc1, queue_slots, 2 );

    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x00, "hw_sc_queue_get() on empty queue does not return 0x00" );
    hw_sc_queue_put( &sc1, 0x01 );
    hw_sc_queue_put( &sc1, 0x02 );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x01, "hw_sc_queue_get() on queue with two entries does not return 0x01" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x02, "hw_sc_queue_get() on queue with one entries does not return 0x02" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x00, "hw_sc_queue_get() on empty queue after emptying set one does not return 0x00" );

    hw_sc_queue_put( &sc1, 0x03 );
    hw_sc_queue_put( &sc1, 0x04 );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x03, "hw_sc_queue_get() on queue with two entries does not return 0x03" );
    hw_sc_queue_put( &sc1, 0x05 );
    hw_sc_queue_put( &sc1, 0x06 );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x04, "hw_sc_queue_get() on queue with two entries does not return 0x04" );
    hw_sc_queue_put( &sc1, 0x07 );
    hw_sc_queue_put( &sc1, 0x08 );
    hw_sc_queue_put( &sc1, 0x09 ); // should wrap here
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x06, "hw_sc_queue_get() on queue with four entries does not return 0x06 after wrap" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x07, "hw_sc_queue_get() on queue with three entries does not return 0x07" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x08, "hw_sc_queue_get() on queue with two entries does not return 0x08" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x09, "hw_sc_queue_get() on queue with one entries does not return 0x09" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x00, "hw_sc_queue_get() on queue with zero entries does not return 0x00" );

    hw_sc_queue_put( &sc1, 0x0a );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x0a, "hw_sc_queue_get() on queue with one entries does not return 0x0a" );
    fail_unless( s, hw_sc_queue_get( &sc1 ) == 0x00, "hw_sc_queue_get() on queue with zero entries does not return 0x00" );

    return conclude_suite( s );
}

