#ifndef PLATFORM_GDT
#define PLATFORM_GDT

typedef struct gdt_ptr {
    u16 limit;
    u64 base_addr;
} __attribute__((packed)) gdt_ptr;


typedef struct gdt_entry {
    u16 limit_low;
    u16 base_addr_low;
    u8  base_addr_mid;
    u8  flags_low;
    u8  limit_high_and_flags_high;
    u8  base_addr_high;
} __attribute__((packed)) gdt_entry;


void gdt_init( void );
void gdt_install( void );
void gdt_segment_refresh( u8 data_segment, u8 code_segment );


#endif
