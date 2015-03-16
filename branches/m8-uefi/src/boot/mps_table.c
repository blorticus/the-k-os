#include <boot/mps_table.h>


void printMPSTableInfo( term_entry* te, MpFloatingStructure* mps_floating_structure ) {
    kterm_printf( te, "MPS Floating Structure\n  -- signature = %8x, length = %2x, spec_rev = %2x, mp_feature_bytes = %2x:%8x\n",
                    mps_floating_structure->signature, mps_floating_structure->length, mps_floating_structure->spec_rev,
                    mps_floating_structure->mp_feature_byte_1, mps_floating_structure->mp_feature_bytes_2_5 );
}
