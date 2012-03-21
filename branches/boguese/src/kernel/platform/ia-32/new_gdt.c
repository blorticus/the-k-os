#include <platform/ia-32/gdt.h>
#include <platform/ia-32/tss.h>
#include <sys/types.h>

/* Sett include/kernel/platform/ia-32/gdt.h for details of the GDT structure */
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


struct gdt_table_ptr {
    u16 size;
    u32 table_base_ptr;
}__attribute__((packed));

static gdt_entry gdt_table[GDT_TABLE_SIZE];
struct gdt_table_ptr gdt_table_info;


// XXX: move this somewhere more general
void raise_gpf( u8 error_code ) {
    asm( "push %%eax\n\t"
         "int  $13"
         : 
         : "a" (error_code)
       );
}


// we call this repeatedly here, so we make this very small optimization
static inline void add_cpu_tss_to_gdt( u8 cpu_num  ) {
    u32 tssp = (u32)(&tss_table[cpu_num - 1]);
    int i = GDT_TABLE_BASE_SIZE + cpu_num - 1;

    gdt_table[i].limit      = (u16)(sizeof( struct tss ));
    gdt_table[i].base_low   = (u16)tssp;
    gdt_table[i].base_mid   = (u8)(tssp >> 16);
    gdt_table[i].flags_and_limit_high = GDT_FLAG_IS_TSS | GDT_FLAG_4K_GRANULAR | GDT_FLAG_PRESENT | GDT_FLAG_DPL_0;
    gdt_table[i].base_high  = (u8)(tssp >> 24);
}


/**
 * DESCRIPTION          : Add a segment descriptor to the GDT.
                          'table' is a pointer to the GDT.  'element_offset' is the descriptor base offset (must be multiple of 8).
 *                        'addr' is the selector base address.  'limit' is the selector limit (size).  'flags' is the logical-or
 *                        of 0x0000 and appropriate GDT_FLAG_* values (the negation of a value -- e.g., is not code segment -- does not
 *                        need to be included, and so are omitted).  'max_table_entries' is the maximum number of entries in the GDT.
 * RETURNS              : void
 * ERRORS               : if 'element' is not a multiple of eight, is beyond 'max_table_entries' or sets a
 *                        segment selector beyond 'max_table_entries' raise #GP
 */
static void set_gdt_segment_descriptor( gdt_entry* table, u16 element_offset, u32 addr, u16 limit, u16 flags, u16 max_table_entries ) {
    int i;

    if (element_offset > (max_table_entries - 1) * 8 || element_offset % 8 != 0) {
        raise_gpf( 0 );
        return;
    }

    i = element_offset >> 3;  // element_offset / 8

    table[i].limit       = limit;
    table[i].base_low    = (u16)addr;
    table[i].base_mid    = (u8)(addr >> 16);
    table[i].flags_and_limit_high = flags;      // flags have limit mid built-in
    table[i].base_high   = (u8)(addr >> 24);
}

//void set_gdt_segment_descriptor( u16 element_offset, u32 addr, u16 limit, u16 flags ) {
//    int i;
//
//    if (element_offset > (GDT_TABLE_SIZE - 1) * 8 || element_offset % 8 != 0) {
//        raise_gpf( 0 );
//        return;
//    }
//
//    i = element_offset >> 3;  // element_offset / 8
//
//    gdt_table[i].limit       = limit;
//    gdt_table[i].base_low    = (u16)addr;
//    gdt_table[i].base_mid    = (u8)(addr >> 16);
//    gdt_table[i].flags_and_limit_high = flags;      // flags have limit mid built-in
//    gdt_table[i].base_high   = (u8)(addr >> 24);
//}


/**
 * DESCRIPTION          : Create GDT with a null descriptor, a code + data descriptor for DPL 0, a code + data descriptor for DPL 3,
 *                        then one TSS per CPU up to MAX_CPUS, with GDT descriptor offsets in that order.  All of this sets values
 *                        in gdt_table
 *                        'gtp' is the GDT register structure.  'gtable' is a pointer to the GDT.
 * RETURNS              : A pointer to the GDT table 
 * ERRORS               : -
 */
static void create_system_canonical_gdt( struct gdt_table_ptr gtp, gdt_entry* gtable ) {
    int i;

    gtp.size           = sizeof(gdt_table) - 1;
    gtp.table_base_ptr = (u32)gdt_table;

    set_gdt_segment_descriptor( gtable, GDT_NULL_SELECTOR_OFFSET,       0, 0,       0x00, GDT_TABLE_SIZE );
    set_gdt_segment_descriptor( gtable, GDT_DPL_0_CODE_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_CODE_SEGMENT | 
                                                                                    GDT_FLAG_IS_READABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_32BIT_SIZE, GDT_TABLE_SIZE );
    set_gdt_segment_descriptor( gtable, GDT_DPL_0_DATA_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_DATA_SEGMENT | 
                                                                                    GDT_FLAG_IS_WRITABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_4_GiB_UPPER_BOUND, GDT_TABLE_SIZE );
    set_gdt_segment_descriptor( gtable, GDT_DPL_3_CODE_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_3 | GDT_FLAG_IS_CODE_SEGMENT | 
                                                                                    GDT_FLAG_IS_READABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_32BIT_SIZE, GDT_TABLE_SIZE);
    set_gdt_segment_descriptor( gtable, GDT_DPL_3_DATA_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_3 | GDT_FLAG_IS_DATA_SEGMENT | 
                                                                                    GDT_FLAG_IS_WRITABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_4_GiB_UPPER_BOUND, GDT_TABLE_SIZE );

    for (i = 1; i <= MAX_CPUS; i++)
        add_cpu_tss_to_gdt( i );


//    gdt_table_info.size = sizeof(gdt_table) - 1;
//    gdt_table_info.table_base_ptr = (u32)gdt_table;
//
//    set_gdt_segment_descriptor( GDT_NULL_SELECTOR_OFFSET,       0, 0,       0x00 );
//    set_gdt_segment_descriptor( GDT_DPL_0_CODE_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_CODE_SEGMENT | 
//                                                                            GDT_FLAG_IS_READABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_32BIT_SIZE );
//    set_gdt_segment_descriptor( GDT_DPL_0_DATA_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_DATA_SEGMENT | 
//                                                                            GDT_FLAG_IS_WRITABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_4_GiB_UPPER_BOUND );
//
//    set_gdt_segment_descriptor( GDT_DPL_3_CODE_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_3 | GDT_FLAG_IS_CODE_SEGMENT | 
//                                                                            GDT_FLAG_IS_READABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_32BIT_SIZE );
//    set_gdt_segment_descriptor( GDT_DPL_3_DATA_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_3 | GDT_FLAG_IS_DATA_SEGMENT | 
//                                                                            GDT_FLAG_IS_WRITABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_4_GiB_UPPER_BOUND );
//
//
//    for (i = 1; i <= MAX_CPUS; i++)
//        _add_cpu_tss_to_gdt( i );
}


void install_standard_gdt() {
    create_system_canonical_gdt( gdt_table_info, gdt_table );
    install_gdt();
}
