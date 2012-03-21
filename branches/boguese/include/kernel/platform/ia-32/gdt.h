#ifndef GDT
#define GDT

#include <sys/types.h>
#include <platform/ia-32/cpus.h>

#define GDT_TABLE_BASE_SIZE                 5   // null selector, code for priv lvl 0, data for pl 0, code for pl 3, data for pl 3
#define GDT_NULL_SELECTOR_OFFSET            0x00
#define GDT_DPL_0_CODE_SEGMENT_OFFSET       0x08
#define GDT_DPL_0_DATA_SEGMENT_OFFSET       0x10
#define GDT_DPL_3_CODE_SEGMENT_OFFSET       0x18
#define GDT_DPL_3_DATA_SEGMENT_OFFSET       0x20


/**
 * DESCRIPTION          : Create GDT with a null descriptor, one descriptor each for DPL 0 code, DPL 0 data, DPL 3 code, DPL 3 data
 *                        and one TSS for each possible CPU (as defined by MAX_CPUS)
 * RETURNS              : void
 * ERRORS               : -
 */
void install_standard_gdt();


#endif
