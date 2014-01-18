#ifndef __KCOMMONDS__
#define __KCOMMONDS__

/**
 * DESCRIPTION      : definitions common to different kernel data structures
 */


/**
 * The kernel_common_ds_node is a data structure node that is used in various
 * kernel data structures.  It is common so that it can be passed between
 * data structures.
 */
typedef struct kernel_common_ds_node {
    struct kernel_common_ds_node* next;
    struct kernel_common_ds_node* prev;
    void* data;
} kernel_common_ds_node;



#endif
