#ifndef BOOT__MPS_TABLE
#define BOOT__MPS_TABLE

#include <video/text_terminal.h>

typedef struct {
    UINT32 signature;
    UINT32 physical_addr_ptr;
    UINT8  length;
    UINT8  spec_rev;
    UINT8  checksum;
    UINT8  mp_feature_byte_1;
    UINT32 mp_feature_bytes_2_5;
} MpFloatingStructure;


typedef struct {
    UINT32 signature;
    UINT16 base_table_length;
    UINT8  spec_rev;
    UINT8  checksum;
    UINT32 oem_word_1;
    UINT32 oem_word_2;
    UINT32 prod_id_word_1;
    UINT32 prod_id_word_2;
    UINT32 prod_id_word_3;
    UINT32 oem_table_ptr_address;
    UINT16 oem_table_size;
    UINT16 entry_count;
    UINT32 local_apic_memmap_address;
    UINT16 extended_table_length;
    UINT8  extended_table_checksum;
    UINT8  reserved0;
} MpConfigTableHeader;


void printMPSTableInfo( term_entry* te, MpFloatingStructure* mps_floating_structure );

#endif
