#include <memory/kmalloc.h>
#include <sys/types.h>
#include <memory/paging.h>

// XXX: ACTUALLY, NEED TO ADD PAGING METHOD TO RETURN MULTIPLE CONTIGUOUS VIRTUAL PAGES
void* kmalloc( size_t len ) {
    if (len >= 4096)
        return NULL;
    return (void*)allocate_virtual_page();
}


// XXX: AGAIN, BECAUSE WE DON'T TRACK HOW MUCH MEMORY IS ALLOCATED, WE DON'T KNOW HOW MUCH TO FREE, SO ONLY FREE ONE PAGE
void kfree( void* m ) {
    deallocate_virtual_page( m );
}


void* kmalloc_page() {
    return (void*)allocate_virtual_page();
}
