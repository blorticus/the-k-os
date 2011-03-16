#ifndef __KCIRC_LIST__
#define __KCIRC_LIST__

#include <util/kcommonds.h>

typedef struct kcirc_list {
    kernel_common_ds_node* current;
} kcirc_list;

inline void kcirc_list_init( kcirc_list* kl );
inline void kcirc_list_add ( kcirc_list* kl, kernel_common_ds_node* node );
inline int  kcirc_list_is_empty( kcirc_list* kl );
kernel_common_ds_node* kcirc_list_advance( kcirc_list* kl );
kernel_common_ds_node* kcirc_list_remove ( kcirc_list* kl );

#endif
