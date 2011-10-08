#include <lcheck.h>
#include <util/kcirc_list.h>
#include <sys/types.h>

int main( void ) {
    lcheck_suite* s;
    kcirc_list _kl;
    kcirc_list* kl = &_kl;
    kernel_common_ds_node nodes[10];

    s = create_suite( "kcirc_list" );

    kcirc_list_init( kl );

    fail_unless( s, kcirc_list_is_empty( kl ), "On init, list is not empty" );
    fail_unless( s, kcirc_list_advance( kl ) == NULL, "On initial empty list, list_advance does not return NULL" );
    fail_unless( s, kcirc_list_remove_current( kl ) == NULL, "On initial empty list, list_remove does not return NULL" );
    fail_unless( s, kcirc_list_advance( kl ) == NULL, "On initial empty list, second list_advance does not return NULL" );
    fail_unless( s, kcirc_list_remove_current( kl ) == NULL, "On initial empty list, second list_remove does not return NULL" );

    kcirc_list_add( kl, &nodes[0] );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After first add, list is empty" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[0], "After first add, advance does not return single added node" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[0], "After first add, second advance does not return single added node" );

    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[0], "After first add then remove, removed node is not single added node" );
    fail_unless( s, kcirc_list_is_empty( kl ), "After first add then remove, list is not empty" );
    fail_unless( s, kcirc_list_advance( kl ) == NULL, "After first add then remove, list_advance does not return NULL" );
    fail_unless( s, kcirc_list_remove_current( kl ) == NULL, "After first add then remove, list_remove does not return NULL" );
    fail_unless( s, kcirc_list_advance( kl ) == NULL, "After first add then remove, second list_advance does not return NULL" );
    fail_unless( s, kcirc_list_remove_current( kl ) == NULL, "After first add then remove, second list_remove does not return NULL" );

    kcirc_list_add( kl, &nodes[1] );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After first add second time, list is empty" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[1], "After first add second time, advance does not return single added node" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[1], "After first add second time, second advance does not return single added node" );

    kcirc_list_add( kl, &nodes[2] );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After second add second time, list is empty" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After second add second time, advance does not return second added node" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[1], "After first add second time, second advance does not return first added node" );

    kcirc_list_add( kl, &nodes[3] );
    kcirc_list_add( kl, &nodes[4] );
    kcirc_list_add( kl, &nodes[5] );

    // should now be 2 - 3 - 4 - 5 - 1  with current -> 1

    fail_unless( s, !kcirc_list_is_empty( kl ), "After next three adds second time, list is empty" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After next three adds second time, first advance does not return nodes[2]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[3], "After next three adds second time, second advance does not return nodes[3]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[4], "After next three adds second time, third advance does not return nodes[4]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[5], "After next three adds second time, fourth advance does not return nodes[5]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[1], "After next three adds second time, fifth advance does not return nodes[1]" );

    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[1], "After next three adds second time, first remove does not return nodes[1]" );
    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[5], "After next three adds second time, second remove does not return nodes[5]" );

    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After first removes second time, first advance does not return nodes[2]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[3], "After first removes second time, second advance does not return nodes[3]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[4], "After first removes second time, third advance does not return nodes[4]" );

    // should now be 2 - 3 - 4  with current -> 4
    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[4], "After second removes second time, first remove does not return nodes[4]" );
    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[3], "After second removes second time, second remove does not return nodes[3]" );
    fail_unless( s, kcirc_list_remove_current( kl ) == &nodes[2], "After second removes second time, third remove does not return nodes[2]" );

    fail_unless( s, kcirc_list_is_empty( kl ), "After second removes second time, list is not empty" );

    kcirc_list_init( kl );

    fail_unless( s, kcirc_list_is_empty( kl ), "After init on list again, list is not empty" );

    kcirc_list_add( kl, &nodes[0] );
    kcirc_list_add( kl, &nodes[1] );
    kcirc_list_add( kl, &nodes[2] );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After three node adds, list is empty" );
    fail_unless( s, kcirc_list_peek_current( kl ) == &nodes[0], "After three node adds, current is not first added node" );

    fail_unless( s, kcirc_list_remove_node( kl, &nodes[1] ) == &nodes[1], "List remove on nodes[1] did not return pointer to nodes[1]" );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After one node remove, list is empty" );
    fail_unless( s, kcirc_list_peek_current( kl ) == &nodes[0], "After one node remove, current is not first added node" );

    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After removal of nodes[1], advance does not move to nodes[2]" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After removal of nodes[1], second advance does not move to nodes[0]" );

    fail_unless( s, kcirc_list_remove_node( kl, &nodes[0] ) == &nodes[0], "List remove of current node -- nodes[0] -- did not return nodes[0]" );

    fail_unless( s, !kcirc_list_is_empty( kl ), "After second node remove, list is empty" );
    fail_unless( s, kcirc_list_peek_current( kl ) == &nodes[2], "After second node remove, current is not final remaining node" );
    fail_unless( s, kcirc_list_advance( kl ) == &nodes[2], "After removal of nodes[0], advance does not move to final existing node" );

    fail_unless( s, kcirc_list_remove_node( kl, &nodes[2] ) == &nodes[2], "List remove of final node -- nodes[2] -- did not return nodes[2]" );

    fail_unless( s, kcirc_list_is_empty( kl ), "After removal of final node, list is not empty" );

    return conclude_suite( s );
}

