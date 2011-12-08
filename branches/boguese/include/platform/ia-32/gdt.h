#ifndef GDT
#define GDT

#include <sys/types.h>
#include <platform/ia-32/cpus.h>

/**
 *
 * The Global Descriptor Table (GDT) is explained in the Intel® 64 and IA-32
 * Architectures Software Developer’s Manual Volume 3A.  It is a table of
 * segment descriptors.  Each descriptor is 8 bytes, and varies according to
 * the descriptor type.  Segment descriptors provide basic access privileges
 * and segment configuration.  In systems that do not use segments as the
 * primary means of memory protection (like this OS), only a few descriptors
 * are required.  Specifically, there must be a null segment descriptor, both a
 * code and a data segment descriptor for each processor privilege level used,
 * and one Task Segment Selector (TSS) descriptor for each CPU.  This OS uses
 * privilege level 0 for kernel mode and privilege level 3 for user mode.
 * Levels 1 and 2 are not used.  This maximizes portability, since many
 * processors only have two privilege levels, and the use of additional
 * privilege levels increases complexity with only marginal gain.  GDT entries
 * are indexed by their byte offset in the table (so the first entry is 0x0,
 * the second is 0x8, the third is 0x10, etc).
 *
 * The null segment selector is required by the processor, and must be the
 * first segment descriptor (0x0).  Following convention, the second segment
 * descriptor (0x08) will be used for the code segment of DPL (Descriptor
 * Privilege Level) 0; the third segmeent selector (0x10) will be used for the
 * data segment of DPL 0; the fourth (0x18) will be used for the DPL 3 code
 * segment; the fifth (0x20) will be used for the DPL 3 data segment; the sixth
 * (0x28) will be used for the first CPU TSS; and subsequent segments will be
 * used for the other CPU TSS's -- one per CPU.
 *
 * A segment descriptor for a code segment looks like this:
 *
 * 0                8               16               24               32
 * |----------------|----------------|----------------|----------------|
 * | Limit Low                       | Base Low                        |
 * |----------------|----------------|----------------|----------------|
 * | Base Mid       |G|D|0|X|Lim Mid |P|DPL |1|1|C|R|A| Base High      |
 * |----------------|----------------|----------------|----------------|
 *
 * With a flat memory model, the "code" segment for DPL 0 is all of linear
 * memory.  In other words, a code segment can point to code instructions
 * anywhere in memory, and that is valid.  This is because this OS does not use
 * segments for memory protection (instead it uses paging without segmentation
 * protection).  The base memory address allowed for this segment (DPL 0 code
 * segment) is 0 (start of memory), and the limit is the end of memory (IA-32
 * without PAE can address up to 4 GiB of memory).  The flags are:
 *
 * [A] accessed; it is changed by the processor
 * [R] read-enabled; if false, can only be used in CS; if true, can be used
 *     in a data segment, indicating that code and static data are mixed (as
 *     one might find in a ROM image)
 * [C] conforming; if true; execution can proceed at a higher privilege level;
 *     if false, an attempt to do so will raise #GP
 * [DPL] aforementioned privilege level
 * [P] present; attempt to access when this is cleared (i.e., not present)
 *     raises #NP (Not Present exception).  This can be used by OS for memory
 *     virtualization.
 * [X] can be used by OS for anything (but not used by this OS)
 * [D] default operand size; if set, operands for instructions operands and
 *     effective addresses are assumed to be 32-bit; if cleared, assumed to be
 *     16-bit.
 * [G] granularity of segment limit scaling; if set, limit is interpreted as
 *     a multiple of 4 kiB; if clear, it is interpreted as bytes.  To address
 *     all of linear memory (4 GiB), this must be set.
 *
 * So, for the DPL 0 code segment, the flags will be:
 *    1100 [0000] 1001 1010
 * and thus, with [0000] as Lim-Mid, this is:
 *   0x  c      0    9    a
 * Remembering that byte order is little endian for each 16-bit word, this is:
 *   0xc09a
 *
 * A segment descriptor for a data segment looks like this:
 *
 * 0                8               16               24               32
 * |----------------|----------------|----------------|----------------|
 * | Limit Low                       | Base Low                        |
 * |----------------|----------------|----------------|----------------|
 * | Base Mid       |G|B|0|X|Lim Mid |P|DPL |1|0|E|W|A| Base High      |
 * |----------------|----------------|----------------|----------------|
 *  
 * Again, for a flat-model, the DPL 0 data segment is all of linear
 * memory.  Limit and Base have the same meaning as for code segments,
 * and all of the flags are the same except:
 *
 * [W] writable; true if the segment can be written to; false if nota
 *     Executable images are generally divided into three sections:
 *     text (code), data, and stack.  In this setup, the data segment
 *     is all pre-allocated data (i.e., global variables and arrays),
 *     while the stack is for the formation of procedure stack frames,
 *     including local variables and registers.  In this system, data
 *     segments need not be writable, while stack segments must be.
 *     For the OS, the data segment isn't used in this way, and is really
 *     all of memory.  Therefore, it must be writable.
 * [E] expand down; if true, the base of the segment (address 0) is
 *     the top of segment memory (base + limit).  This would be used for an
 *     image stack segment; if false, the base of the segment is 'base',
 *     as would be the case for a typeical 'data' segment.
 * [B] upper bound; if true, upper bound is 4GiB, as would be true for
 *     a data segment; if false, upper bound is 1 MiB, as would be true
 *     for a stack segment (since it grows "down").
 *
 * So, for the DPL 0 data segment, the flags will be:
 *    1100 [0000] 1001 0010
 * and thus, with [1111] as Lim-Mid, this is:
 *   0x  c      0    9    2
 * Remembering that byte order is little endian for each 16-bit word, this is:
 *   0xc092
 *
 * For DPL 3 code and data segments, the base, limit and flags are all the
 * same as for DPL 0, except of course, the DPL flags themselves.
 *
 * So, for the DPL 3 code segment, the flags will be:
 *    1100 [0000] 1111 1010
 * and thus, if Lim-Mid is all ones, this is:
 *    0xc0fa
 *
 * For the DPL 3 data segment, the flags will be: 
 *    1100 [0000] 1111 0010
 * and thus, if Lim-Mid is all ones, this is:
 *    0xc0f2
 *
 * Task Segement Selectors (TSS) are nominally used for hardware assisted multi-tasking.
 * The literature suggests that the actual performance boost from using the TSS is low
 * to none, and the use of TSS reduces flexibility and restricts the number of tasks
 * that are possible.  However, the processor will use two elements of a TSS when it
 * must perform a task switch.  Specifically, it restores SS and ESP.  In a system
 * that uses software multi-tasking, the only task switch the processor "knows" about
 * is when an interrupt fires (and the privilege changes from 3 to 0).  Since each
 * processor maintains its own system stack, there must be one TSS entry in the GDT
 * is required for each processor on the system.  This OS loads one TSS in the GDT
 * for each processor that could be supported.
 *
 * The structure of a TSS descriptor is:
 *
  * 0                8               16               24               32
 * |----------------|----------------|----------------|----------------|
 * | Limit Low                       | Base Low                        |
 * |----------------|----------------|----------------|----------------|
 * | Base Mid       |G|0|0|X|Lim Mid |P|DPL |0|1|0|B|1| Base High      |
 * |----------------|----------------|----------------|----------------|
 *
 * [G], [P], [X] are the same as for code segments and data
 * segments.  However, 'base' is the linear address of the TSS entry.
 * 'limit' is the size of the entry.  Furthermore:
 *
 * [B] is set if the task represented by the TSS is active; cleared otherwise.
 * [DPL] is the privilege level allowed to perform the task switch
 *
 * The flag set for each TSS entry, then, is:
 *   1000 [0000] 1000 1001
 * that is:
 *  0x8089
 */

/* Common to CODE, DATA and TSS */
#define GDT_FLAG_4K_GRANULAR        0x8000  // 1000 0000 0000 0000
#define GDT_FLAG_BYTE_GRANULAR      0x0000  // 0000 0000 0000 0000
#define GDT_FLAG_PRESENT            0x0080  // 0000 0000 1000 0000
#define GDT_FLAG_DPL_0              0x0000  // 0000 0000 0000 0000
#define GDT_FLAG_DPL_1              0x0020  // 0000 0000 0010 0000
#define GDT_FLAG_DPL_2              0x0040  // 0000 0000 0100 0000
#define GDT_FLAG_DPL_3              0x0060  // 0000 0000 0110 0000

/* Common to CODE, DATA */
#define GDT_FLAG_32BIT_SIZE         0x4000  // 0100 0000 0000 0000
#define GDT_FLAG_16BIT_SIZE         0x0000  // 0000 0000 0000 0000
#define GDT_FLAG_4_GiB_UPPER_BOUND  0x4000  // 0100 0000 0000 0000
#define GDT_FLAG_1_MiB_UPPER_BOUND  0x0000  // 0000 0000 0000 0000

/* CODE only */
#define GDT_FLAG_IS_CODE_SEGMENT    0x0018  // 0000 0000 0001 1000
#define GDT_FLAG_IS_CONFORMING      0x0004  // 0000 0000 0000 0100
#define GDT_FLAG_IS_READABLE        0x0002  // 0000 0000 0000 0010

/* DATA only */
#define GDT_FLAG_IS_DATA_SEGMENT    0x0010  // 0000 0000 0001 0000
#define GDT_FLAG_EXPAND_DOWN        0x0004  // 0000 0000 0000 0100
#define GDT_FLAG_IS_WRITABLE        0x0002  // 0000 0000 0000 0010

#define GDT_FLAG_IS_SYSTEM_SEGMENT  0x0000  // 0000 0000 0000 0000

/* TSS only */
#define GDT_FLAG_IS_TSS             0x0009  // 0000 0000 0000 1001
#define GDT_TASK_IS_BUSY            0x0002  // 0000 0000 0000 0010


typedef struct gdt_entry {
    u16 limit;
    u16 base_low;
    u8  base_mid;
    u16 flags_and_limit_high;
    u8  base_high;
}__attribute__((packed)) gdt_entry;

#define MAX_GDT_SEGMENTS                    8192
#define GDT_TABLE_BASE_SIZE                 5   // null selector, code for priv lvl 0, data for pl 0, code for pl 3, data for pl 3
#define GDT_NULL_SELECTOR_OFFSET            0x00
#define GDT_DPL_0_CODE_SEGMENT_OFFSET       0x08
#define GDT_DPL_0_DATA_SEGMENT_OFFSET       0x10
#define GDT_DPL_3_CODE_SEGMENT_OFFSET       0x18
#define GDT_DPL_3_DATA_SEGMENT_OFFSET       0x20
#define GDT_TSS_SEGMENT_OFFSET_FOR_CPU(cpu) (0x28 + ((cpu - 1) * 8))

#define GDT_TABLE_SIZE                      GDT_TABLE_BASE_SIZE + MAX_CPUS

gdt_entry gdt_table[GDT_TABLE_SIZE];

/**
 * DESCRIPTION          : Add a segment descriptor to the GDT.  'element_offset' is the descriptor base offset (must be multiple of 8).
 *                        'addr' is the selector base address.  'limit' is the selector limit (size).  'flags' is the logical-or
 *                        of 0x0000 and appropriate GDT_FLAG_* values (the negation of a value -- e.g., is not code segment -- does not
 *                        need to be included, and so are omitted).
 * RETURNS              : void
 * ERRORS               : if 'element' is not a multiple of eight, is beyond the GDT table limit ((MAX_GDT_SEGMENTS - 1) * 8) or sets a
 *                        segment selector beyond the GDT table size ((GDT_TABLE_BASE_SIZE + MAX_CPUS - 1) * 8) raise #GP
 */
void set_gdt_segment_descriptor( u16 element_offset, u32 addr, u16 limit, u16 flags );


/**
 * DESCRIPTION          : Add a task segment selector to the GDT for a particular CPU.  Assumes that GDT_TSS_SEGMENT_OFFSET_FOR_CPU()
 *                        macro returns the proper GDT entry offset for cpu numbered 'cpu_num' (where CPUs are numbered starting at 1)
 * RETURNS              : void
 * ERRORS               : if 'cpu_num' > MAX_CPUS raise #GP
 */
void add_cpu_tss_to_gdt( u8 cpu_num  );


/**
 * DESCRIPTION          : Create GDT with a null descriptor, a code + data descriptor for DPL 0, a code + data descriptor for DPL 3,
 *                        then one TSS per CPU up to MAX_CPUS, with GDT descriptor offsets in that order.  All of this sets values
 *                        in gdt_table
 * RETURNS              : void
 * ERRORS               : -
 */
void create_system_canonical_gdt();
 

/**
 * DESCRIPTION          : Installs gdt_table as the system GDT
 * RETURNS              : void
 * ERRORS               : -
 */
void install_gdt();

#endif
