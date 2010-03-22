#include <multiboot.h>

struct multiboot_relocated_info* MB_RELOCATED_INFO = 0;

void multiboot_relocate() {
    asm volatile ( "movl %%ebx, %0" : "=r"(MB_RELOCATED_INFO) );
}

/* retrieve the cached pointer to the multiboot info */
struct multiboot_relocated_info* retrieve_multiboot_relocate_info() {
    return MB_RELOCATED_INFO;
}

