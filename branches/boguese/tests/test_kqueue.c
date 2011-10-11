#include <lcheck.h>
#include <util/kqueue.h>
#include <sys/types.h>

void test_dequeue( lcheck_suite* s, kqueue* kq, int node_num, kernel_common_ds_node* removing_node, int is_empty_after_remove );

int main( void ) {
    kqueue  _kq;
    kqueue* kq = &_kq;
    kernel_common_ds_node nodes[10];
    kernel_common_ds_node* node;

    lcheck_suite* s;

    s = create_suite( "kqueue" );

    kqueue_init( kq );

    fail_unless( s, kqueue_is_empty ( kq ),            "Initialized kqueue is not empty" );
    fail_unless( s, kqueue_dequeue  ( kq ) == NULL,    "On empty kqueue, kqueue_dequeue() does not return NULL" );

    kqueue_enqueue( kq, &nodes[0] );

    fail_unless( s, !kqueue_is_empty( kq ),             "After single node add, kqueue_is_empty() reports true" );

    kqueue_enqueue( kq, &nodes[1] );
    kqueue_enqueue( kq, &nodes[2] );
    kqueue_enqueue( kq, &nodes[3] );

    fail_unless( s, !kqueue_is_empty( kq ),             "After three more adds, kqueue_is_empty() reports true" );

    node = kqueue_peek( kq );

    fail_unless( s, node == &node[0],                   "After four adds, current node isn't first added node" );

    node = kqueue_dequeue( kq );

    fail_unless( s, node == &nodes[0],                  "On remove after four adds, removed node is not node[0]" );
    fail_unless( s, kqueue_peek( kq ) == &nodes[1],     "On remove after four adds, current isn't node[1]" );

    node = kqueue_dequeue( kq );

    fail_unless( s, node == &nodes[1],                  "On second remove after four adds, removed node is not node[1]" );
    fail_unless( s, kqueue_peek( kq ) == &nodes[2],     "On second remove after four adds, current isn't node[2]" );

    kqueue_enqueue( kq, &nodes[4] );
    kqueue_enqueue( kq, &nodes[0] );

    fail_unless( s, kqueue_peek( kq ) == &nodes[2],     "On add of node 4 and 0, current isn't node[1]" );

    test_dequeue( s, kq, 2, &nodes[2], 0 );
    test_dequeue( s, kq, 3, &nodes[3], 0 );
    test_dequeue( s, kq, 4, &nodes[4], 0 );
    test_dequeue( s, kq, 0, &nodes[0], 1 );

    fail_unless( s, kqueue_dequeue  ( kq ) == NULL,    "After all nodes dequeued, kqueue_dequeue does not return null" );

    return conclude_suite( s );
}


char buf[150];
void test_dequeue( lcheck_suite* s, kqueue* kq, int removing_node_num, kernel_common_ds_node* removing_node, int is_empty_after_remove ) {
    kernel_common_ds_node* node;

    __sprintf( buf, "On dequeue, node should be node[%d] but isn't", removing_node_num );
    node = kqueue_dequeue( kq );
    fail_unless( s, node == removing_node, buf );

    __sprintf( buf, "After dequeue of node %d queue %s be empty but %s", removing_node_num, (is_empty_after_remove ? "should" : "should not"), (is_empty_after_remove ? "isn't" : "is") );
    if (is_empty_after_remove)
        fail_unless( s, kqueue_is_empty( kq ), buf );
    else
        fail_unless( s, !kqueue_is_empty( kq ), buf );
}

