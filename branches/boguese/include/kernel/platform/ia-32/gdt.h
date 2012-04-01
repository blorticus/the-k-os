#ifndef GDT
#define GDT

#define GDT_NULL_SELECTOR_OFFSET            0x00
#define GDT_DPL_0_CODE_SEGMENT_OFFSET       0x08
#define GDT_DPL_0_DATA_SEGMENT_OFFSET       0x10
#define GDT_DPL_3_CODE_SEGMENT_OFFSET       0x18
#define GDT_DPL_3_DATA_SEGMENT_OFFSET       0x20
#define GDT_TSS_SEGMENT_OFFSET_FOR_CPU(cpu) (0x28 + ((cpu - 1) * 8))



/**
 * DESCRIPTION          : Create a Global Descriptor Table with one data and one code selector for each of
 *                        privilege level 0 and privilege level 3.  Also, add one TSS for each possible
 *                        CPU on the system.  Arrange the code/data descriptors using the offsets above.
 * RETURNS              : void
 * ERRORS               : -
 */
void install_standard_gdt();

#endif
