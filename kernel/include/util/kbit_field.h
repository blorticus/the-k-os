#ifndef __KBIT_FIELD__
#define __KBIT_FIELD__

#include <sys/types.h>

/**
 * DESCRIPTION: a bootlean bit field without datastructure allocation
 * ELEMENTS:    - 'bf' is an array of u32 elements that contain bit fields
 *              - 'bflen' is the number of elements in 'bf'
 **/

typedef struct kbit_field {
    u32* bf;
    u32 bflen;
} kbit_field;

/**
 * DESCRIPTION      : initialize kbit_field object, clearing all bits
 * PARAMS           : 'kbf' is the field data structure.  'bf' is a u32* arrany
 *                    which should have int(bit_fields / 32) + 1 elements) (so, if you need
 *                    45 bit fields, this should be u32[2]; if you need 64 elements,
 *                    this should be u32[3]).  bflen is the number
 *                    of bits in the field that you intend to use.
 * NOTES            : no bounds checking is used, so if an effort is made to
 *                    check or set a bit outside of the bit field area, the results
 *                    are undefined for all operations (is_set, clear and set) bit field starts at
 *                    zero.  So, if there are 35 bits, they are numbered 0 .. 34,
 *                    inclusive
 **/
void kbit_field_init   ( kbit_field *kbf, u32* bf, u32 bflen );


 /**
  * DESCRIPTION     : return 1 if selected bit is set; 0 otherwise
  **/
u32  kbit_field_is_set( kbit_field *kbf, u32 bit );


 /**
  * DESCRIPTION     : clear the bit 'bit' in the field
  **/
void kbit_field_clear  ( kbit_field *kbf, u32 bit );


 /**
  * DESCRIPTION     : clear the bit 'bit' in the field
  **/
void kbit_field_set    ( kbit_field *kbf, u32 bit );


#endif
