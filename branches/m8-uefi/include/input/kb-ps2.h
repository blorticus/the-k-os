#ifndef __INPUT_KB_PS2_H__
#define __INPUT_KB_PS2_H__

#include <sys/types.h>


/**
 * BRIEF:           A basic driver of PS/2 style keyboard (generally emulated)
 * BACKGROUND:      Methods in this package install a keyboard IRQ ISR.  A method must be registered
 *                  to receive scancodes.  The invocation of this method occurs within an ISR
 *                  so it should return as quickly as feasible.  Generally, the callback method
 *                  should be a keyboard driver for a specific keyboard type.
 * SEE:             http://wiki.osdev.org/PS2_Keyboard
 * RE-ENTRANT?:     NO
 ***/


/**
 *
 * Callback method when ISR is invoked and a scancode is retrieved.  On each invocation,
 * up to six hardware scan codes are passed in scancode_list.  scancode_count is the
 * number of codes in the scancode_list.  This method will be invoked in a cli protect,
 * so it should execute as quickly as possible, then return to the caller.
 *
 **/
typedef void (*keyboard_scancode_receiver)( u8* scancode_list, u8 scancode_count );


/**
 *
 * DESCRIPTION:     Initialize system for keyboard PS2 driver
 * ARGUMENTS:       'callback' is the method invoked when scancodes are collected.  The scancode_list
 *                  buffer is not shared nor is it synchronized, so it must not be modified and if the
 *                  values are to be retained after invocation of 'callback', they should be copied
 *                  out of the buffer.
 * RETURN:          void
 * SIDE-EFFECTS:    none
 * NOTES:           none
 * RE-ENTRANT?:     NO
 *
 **/
void kb_ps2_init( keyboard_scancode_receiver callback );


/**
 *
 * DESCRIPTION:     Install the IRQ ISR for keyboard input.  This method populates a global ScCQueue, the
 *                  same one from which read_next_key_stroke() reads
 * RETURN:          void
 * SIDE-EFFECTS:    Replaces ISR for keyboard IRQ
 * NOTES:           none
 * RE-ENTRANT?:     NO
 *
 **/ 
void kb_ps2_irq_install();



#endif
