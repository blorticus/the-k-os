#include <util/kqueue.h>
#include <sys/types.h>

/**
 * DESCRIPTION      : kernel queue (FIFO) data structure.  Utilizes a common node structure, also
 *                    used by the circular list.  This is intended to be used for kernel data
 *                    and the data structure functions themselves do not allocate memory.  The
 *                    node is common so that it can be moved between different data structure
 *                    types.
 */

// CONDITIONS:
//  - if queue->head is NULL, the queue is empty
//  - if a kernel_common_ds_node is altered by caller, results are undefined

inline void kqueue_init( kqueue* q ) {
    q->head = NULL;
    q->tail = NULL;
}


inline int kqueue_is_empty( kqueue* q ) {
    return q->head == NULL;
}


void kqueue_enqueue( kqueue* q, kernel_common_ds_node* n ) {
    if (kqueue_is_empty( q )) {
        q->head = q->tail = n;
        n->prev = n->next = NULL;
    }
    else {
        n->next = q->tail;
        n->prev = NULL;
        q->tail = n;
        n->next->prev = n;
    }
}


kernel_common_ds_node* kqueue_dequeue( kqueue* q ) {
    kernel_common_ds_node* r;

    if (kqueue_is_empty( q ))
        return NULL;
    else {
        r = q->head;
        if ((q->head = q->head->prev))
            q->head->next = NULL;

        return r;
    }
}


kernel_common_ds_node* kqueue_peek( kqueue* q ) {
    return q->head;
}
