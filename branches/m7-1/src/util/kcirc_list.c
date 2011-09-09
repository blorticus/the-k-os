#include <util/kcirc_list.h>
#include <sys/types.h>

/**
 * A circularly linked list without rewind and with iterator interface.  On "advance",
 * move iterator to node after current and return a pointer to the node just moved to.  On "add",
 * insert node before current.  On "remove" return current node and move current to
 * node immediately before removed node (do this because iteration is normally via
 * the "advance" method).  On attempt to "remove" from an empty list,
 * return NULL.  On "advance" against tail of list, iterate to head of list
 **/

// when kl->current is NULL, the list is empty
inline void kcirc_list_init( kcirc_list* kl ) {
    kl->current = NULL;
}

inline int kcirc_list_is_empty( kcirc_list* kl ) {
    return kl->current == NULL;
}

void kcirc_list_add ( kcirc_list* kl, kernel_common_ds_node* node ) {
    if (kcirc_list_is_empty( kl )) {
        kl->current = node->prev = node->next = node;
    }
    else {
        node->next = kl->current;
        node->prev = kl->current->prev;
        node->prev->next = node;
        kl->current->prev = node;
    }
}

kernel_common_ds_node* kcirc_list_advance( kcirc_list* kl ) {
    if (kcirc_list_is_empty( kl ))
        return NULL;

    kl->current = kl->current->next;

    return kl->current;
}


kernel_common_ds_node* kcirc_list_remove_current( kcirc_list* kl ) {
    kernel_common_ds_node* r;

    if (kcirc_list_is_empty( kl ))
        return NULL;

    r = kl->current;

    if (kl->current->next == kl->current) { // list has only one item
        kl->current = NULL;
    }
    else {
        kl->current->prev->next = kl->current->next;
        kl->current->next->prev = kl->current->prev;
        kl->current = kl->current->prev;
    }

    return r;
}


kernel_common_ds_node* kcirc_list_remove_node( kcirc_list* kl, kernel_common_ds_node* node ) {
    // if node == current_node, then simply execute kcirc_list_remove_current
    // otherwise, if list is empty, do nothing
    // otherwise, straighten up pointer for next and prev wrt node
    if (kcirc_list_is_empty( kl ))
        return NULL;

    if (kl->current == node)
        return kcirc_list_remove_current( kl );

    if (node->prev == NULL && node->next == NULL)
        return NULL;

    node->prev->next = node->next;
    node->next->prev = node->prev;

    return node;
}


kernel_common_ds_node* kcirc_list_peek_current( kcirc_list* kl ) {
    return kl->current;
}
