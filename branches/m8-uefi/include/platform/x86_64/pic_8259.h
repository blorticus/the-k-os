#ifndef __PIC_8259__
#define __PIC_8259__

#include <sys/types.h>

/**
 *
 * Code for the 8259 Programmable Interrupt Controller (PIC)
 * See: http://pdos.csail.mit.edu/6.828/2012/readings/hardware/8259A.pdf
 *
 **/

typedef enum {
    PIC_NO_ERROR            = 0,
    PIC_ALIGNMENT_ERROR     = 1
} pic_error;


#define PIC1_COMMAND_IO_PORT    0x20
#define PIC2_COMMAND_IO_PORT    0xa0
#define PIC1_DATA_IO_PORT       0x28
#define PIC2_DATA_IO_PORT       0xa8

// For ICW1, bits at slot 2 and 5..7 are 0 for x86 PC architectures
#define PIC_ICW1_BASE           0x10        // Required for ICW1, regardless of other bits
#define PIC_ICW1_ICW4_NEEDED    0x01        // set to 1 if ICW4 will be provided
#define PIC_ICW1_SINGLE         0x02        // set to 1 if single PIC; 0 if cascading
#define PIC_ICW1_LTIM           0x08        // set to 1 for level triggered interrupts; 0 for edge triggered

#define PIC_ICW4_X86            0x01        // mode is x86
#define PIC_ICW4_AUTO_EOI       0x02        // send End-of-Interrupt automatically after ACK of IRQ
#define PIC_ICW4_BUFFER_MODE    0x04        // master/slave buffer mode on or off
#define PIC_ICW4_SFNM           0x08        // Special Fully Nested Mode


// For OCW2, lowest order 3 bits are interrupt level, used for specific EOI or rotation commands
// Bits 4 and 5 must be zero.  The high-order three bits combined have these meanings:
#define PIC_OCW2_NON_SPECIFIC_EOI           0x20    // 0010: Normal, non-specific EOI command for master and/or slave PIC
#define PIC_OCW2_NOOP                       0x40    // 0100: No-operation
#define PIC_OCW2_SPECIFIC_EOI               0x60    // 0110: Specific EOI when interrupt nesting is disturbed; not the case
                                                    //       in default initialization, which is used here
#define PIC_OCW2_ROT_ON_AUTO_EOI            0x80    // 1000: Set rotation on automatic EOI
#define PIC_OCW2_ROT_ON_NON_SPECIFIC_EOI    0xa0    // 1010: Set rotation on non-specific EOI
#define PIC_OCW2_SET_PRIORITY               0xc0    // 1100: Set priority
#define PIC_OCW2_ROT_ON_SPECIFIC_EOI        0xe0    // 1110: Set rotation on specific EOI


/**
 *
 * Initialize the PIC.  Remaps IRQ interrupts according to the provided formula.
 * NOTE: interrupts MUST be disabled before entering this method
 * For 8259 initialization, a series of three or four Initialization Command Words (ICW)
 * are presented, in order.  The first is sent to each controller's command port.
 * The other two or three (ICW2, ICW3 and ICW4) are sent to the controller's data port,
 * each in turn.
 *
 * @arg         irq_remap_first_interrupt   IRQ0 on first PIC will be mapped to this interrupt number;
 *                                          it is followed by the rest of first PIC IRQs, then second PIC IRQs.
 *                                          Lowest three order bits must be 0.
 * @returns                                 0 on success, PIC_ALIGNMENT_ERROR if 3 lowest order bits are not zero
 *
 **/
pic_error pic_initialize( u8 irq_remap_first_interrupt );


/**
 *
 * Send End-of-Interrupt (EOI) to master PIC or master and slave PIC
 *
 * @arg     irq_num     the number of the IRQ for which the EOI is sent
 * @returns             0 on succes, non-zero on error
 *
 **/
pic_error pic_send_eoi( u8 irq_num );


/**
 *
 * Mask all IRQ lines on master and slave PIC using OCW1
 *
 * @returns             0 on success, non-zero on error
 **/
pic_error pic_mask_all_irqs();


#endif
