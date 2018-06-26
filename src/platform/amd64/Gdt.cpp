#include <platform/amd64/Foundation.h>

extern "C" {
extern void create_gdt(void);
}

void Sys::GDT::PopulateNullSegmentDescriptor(uint64_t *descriptor) {
    *descriptor = 0;
}

void Sys::GDT::PopulateCodeSegmentDescriptor(uint64_t *descriptor, PrivilegeLevels pl) {
    *descriptor = 0x0000000000982000 | ((uint32_t)pl << 24);
}

void Sys::GDT::PopulateDataSegmentDescriptor(uint64_t *descriptor) {
    *descriptor = 0x0000000000800000;
}

void Sys::GDT::Install() {
    create_gdt();
}