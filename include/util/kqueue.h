#ifndef __KQUEUE__
#define __KQUEUE__

#include <util/kcommonds.h>

/**
 * DESCRIPTION      : kernel queue (FIFO) data structure.  Utilizes a common node structure, also
 *                    used by the circular list.  This is intended to be used for kernel data
 *                    and the data structure functions themselves do not allocate memory.  The
 *                    node is common so that it can be moved between different data structure
 *                    types.
 */


typedef struct kqueue {
    struct kernel_common_ds_node* head;
    struct kernel_common_ds_node* tail;
} kqueue;


inline void kqueue_init( kqueue* q );
inline int kqueue_is_empty( kqueue* q );
void kqueue_enqueue( kqueue* q, kernel_common_ds_node* n );
kernel_common_ds_node* kqueue_dequeue( kqueue* q );
kernel_common_ds_node* kqueue_peek( kqueue* q );


#endif
