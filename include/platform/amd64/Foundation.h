#pragma once

#include <cstdint>

namespace Sys {
enum PrivilegeLevels : uint8_t {
    PL0    = 0,
    PL1    = 1,
    PL2    = 2,
    PL3    = 3,
};

class GDT {
    public:
    /** Build a Long mode GDT null segment descriptor. **/
    static void PopulateNullSegmentDescriptor(uint64_t* descriptor);

    /** Build a Long mode GDT code segment descriptor at identified CPL. **/
    static void PopulateCodeSegmentDescriptor(uint64_t* descriptor, PrivilegeLevels pl);

    /** Build a Long mode GDT data segment descriptor. **/
    static void PopulateDataSegmentDescriptor(uint64_t* descriptor);

    /** Install GDT from descriptors. **/
    static void Install();
};
}