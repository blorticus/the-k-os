#include <util/kbit_field.h>


void kbit_field_init( kbit_field *kbf, u32* bf, u32 bflen ) {
    int i;

    for (i = 0; i < bflen / 32 + 1; i++)
        bf[i] = 0;

    kbf->bf    = bf;
    kbf->bflen = bflen;
}


u32 kbit_field_is_set( kbit_field *kbf, u32 bit ) {
    u32 bfelem, bfoff;

    bfelem = bit / 32;
    bfoff  = bit - (bfelem * 32);

    return kbf->bf[bfelem] & (0x80000000 >> bfoff);
}


void kbit_field_clear( kbit_field *kbf, u32 bit ) {
    u32 bfelem, bfoff;

    bfelem = bit / 32;
    bfoff  = bit - (bfelem * 32);

    kbf->bf[bfelem] &= ~(0x80000000 >> bfoff);
}


void kbit_field_set( kbit_field *kbf, u32 bit ) {
    u32 bfelem, bfoff;

    bfelem = bit / 32;
    bfoff  = bit - (bfelem * 32);

    kbf->bf[bfelem] |= (0x80000000 >> bfoff);
}
