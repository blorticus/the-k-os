#ifndef PLATFORM_IDT
#define PLATFORM_IDT

#include <video/text_terminal.h>


/**
 *
 * DESCRIPTION:     Creates all 256 entries that IDT can hold, but points all slots to an ISR that does nothing.
 * RETURN:          void
 * SIDE-EFFECTS:    Populate IDT, but not install (lidt) it
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */
void init_idt( term_entry* te );


/**
 *
 * DESCRIPTION:     Set an entry in the IDT, geneally for installing an ISR.  'num' is the IDT entry number; 'func_addr' is the ISR
 *                  function pointer address; 'sel' is the GDT code selector for the function; and 'flags' is the IDT flags
 * RETURN:          void
 * SIDE-EFFECTS:    Updates IDT entry 'num'
 * NOTES:           Setting 'func_addr' to NULL or some invalid address does not inactivate the IDT slot; it will raise an exception on
 *                  that interrupt
 * RE-ENTRANT?:     NO
 *
 */
void idt_set_entry( u8 num, u64 func_addr, u16 sel, u8 present, u8 dpl, u8 ist );


/**
 *
 * DESCRIPTION:     After creating the IDT (via 'create_idt'), and potentially populating entries (via 'idt_set_entry' -- although,
 *                  this can be done after installation), install the IDT by creating the base pointer structure and calling
 *                  'lidt'
 * RETURN:          void
 * SIDE-EFFECTS:    Instructs processor about idt entry location
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */
void idt_install( void );


void isr_routine_does_not_exist( u64 isr_num );


#endif
