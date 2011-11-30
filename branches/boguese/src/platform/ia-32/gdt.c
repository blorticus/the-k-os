#include <platform/ia-32/gdt.h>
#include <platform/ia-32/tss.h>
#include <sys/types.h>

extern void activate_gdt();     // in gdt.asm

struct gdt_table_ptr {
    u16 size;
    u32 table_base_ptr;
}__attribute__((packed));

struct gdt_table_ptr gdt_table_info;

void raise_gpf() { }

void set_gdt_segment_descriptor( u16 element_offset, u32 addr, u16 limit, u16 flags ) {
    int i;

    if (element_offset > (GDT_TABLE_SIZE - 1) * 8 || element_offset % 8 != 0)
        return raise_gpf();

    i = element_offset >> 3;  // element_offset / 8

    gdt_table[i].limit       = limit;
    gdt_table[i].base_low    = (u16)addr;
    gdt_table[i].base_mid    = (u8)(addr >> 16);
    gdt_table[i].flags_and_limit_high = flags;      // flags have limit mid built-in
    gdt_table[i].base_high   = (u8)(addr >> 24);
}


void initialize_gdt() {
    gdt_table_info.size = sizeof(gdt_table) - 1;
    gdt_table_info.table_base_ptr = (u32)gdt_table;

    set_gdt_segment_descriptor( GDT_NULL_SELECTOR_OFFSET,       0, 0,       0x00 );
    set_gdt_segment_descriptor( GDT_DPL_0_CODE_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_CODE_SEGMENT | 
                                                                            GDT_FLAG_IS_READABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_32BIT_SIZE );
    set_gdt_segment_descriptor( GDT_DPL_0_DATA_SEGMENT_OFFSET,  0, 0xffff,  GDT_FLAG_PRESENT | GDT_FLAG_DPL_0 | GDT_FLAG_IS_DATA_SEGMENT | 
                                                                            GDT_FLAG_IS_WRITABLE | GDT_FLAG_4K_GRANULAR | GDT_FLAG_4_GiB_UPPER_BOUND );

    activate_gdt();
}

// 'cpu_num' starting at 1
// Caller's responsibility to ensure that cpu_num <= MAX_CPUS
void add_cpu_tss_to_gdt( u8 cpu_num  ) {
    u32 tssp = (u32)(&tss_table[cpu_num - 1]);
    int i = GDT_TABLE_BASE_SIZE + cpu_num - 1;

    gdt_table[i].limit      = (u16)(sizeof( struct tss ));
    gdt_table[i].base_low   = (u16)tssp;
    gdt_table[i].base_mid   = (u8)(tssp >> 16);
//    gdt_table[i].access_bits     = 0x89;
    gdt_table[i].flags_and_limit_high = 0xcf;
    gdt_table[i].base_high  = (u8)(tssp >> 24);
}
