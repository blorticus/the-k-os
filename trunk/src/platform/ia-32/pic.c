#include <pic.h>
#include <sys/asm.h>


/* command and data I/O ports for master and slave PIC */
#define MASTER_PIC_CMD_PORT     0x0020
#define MASTER_PIC_DATA_PORT    0x0021
#define SLAVE_PIC_CMD_PORT      0x00A0
#define SLAVE_PIC_DATA_PORT     0x00A1

#define PIC_CMD_END_OF_INTERRUPT    0x20        // signals end of interrupt handling

/* PIC First Initialization Command Word (ICW1) bit position meanings */
#define PIC_ICW1_NEED_ICW4          0x01        // will send ICW4 if true; will not if false
#define PIC_ICW1_SINGLE_PIC         0x02        // single PIC if true; cascaded PICs if false
#define PIC_ICW1_CALL_ADDR_INTVL_4  0x04        // Call address interval 4 if true; 8 if false
#define PIC_ICW1_LVL_TRIGGERED_INTS 0x08        // Level triggered interrupts if true; edge triggered if false
#define PIC_ICW1_INIT               0x10        // Must be true for ICW1
#define PIC_ICW1_MCS_80_INT_ADDR_1  0x20        // Interrupt vector addresses for MCS-80/85 Mode, first bit
#define PIC_ICW1_MCS_80_INT_ADDR_2  0x40        // Interrupt vector addresses for MCS-80/85 Mode, second bit
#define PIC_ICW1_MCS_80_INT_ADDR_3  0x80        // Interrupt vector addresses for MCS-80/85 Mode, third bit

/* PIC Fourth Initialization Command Word (ICW4) bit position meanings */
#define PIC_ICW4_8086_MODE          0x01        // true if 8086 mode; false of MCS 80/85 mode
#define PIC_ICW4_AUTO_EOI           0x02        // true if using auto EOI, false if not
#define PIC_ICW4_BUFFERING          0x04        // true if using slave/master buffering mode; false otherwise
#define PIC_ICW4_SFNM               0x08        // true if using special fully nested mode; false if using sequential
// bits 5..7 are unused in ICW4

inline void PIC_end_of_irq_handling( u8 for_irq ) {
    /* if IRQ came from slave, must signal master and slave; else, only the master */
    if (for_irq > 8)
        ioport_writeb( SLAVE_PIC_CMD_PORT, PIC_CMD_END_OF_INTERRUPT );

    ioport_writeb( MASTER_PIC_CMD_PORT, PIC_CMD_END_OF_INTERRUPT );
}


/**
 * The BIOS initializes the PIC such that IRQs 0-7 (from master PIC) map to
 * interrupts 8 - 15, while IRQs 8 - 16 (from slave PIC) map to interrupts
 * 70 - 77.  This method remaps IRQs -> interrupts, first for the master, then
 * the slave.  IRQ0 will map to pic1_irq_interrupt_base, IRQ1 to pic1_irq_interrupt_base + 1,
 * and so forth.  IRQ8 will be mapped to pic2_irq_interrupt_base, IRQ9 to pic2_irq_interrupt_base + 1,
 * and so forth.
 * To understand a bit better the order of operations, see: http://www.beyondlogic.org/interrupts/interupt.htm
 * Basically, four interrupt command words (8-bits per word) are sent.  The first is sent to each PIC
 * command port; the last three are sent to the data port.  ICW1 and ICW4 bits are outlined above in
 * the defines.  ICW2 remaps the interrupt number.  ICW3 describes which IRQs are used for cascading.
 * For the master, the first bit is true if IRQ0 is used, the second bit is true if IRQ1 is used, and so forth.
 * For the slave, the first bit is true if IRQ8 is used, the second bit is true if IRQ9 is used, and so forth.
 * Normally, IRQ2 and IRQ9 are used on the master and slave, respectively.
 */
void PIC_remap_irq_interrupts( u8 pic1_irq_interrupt_base, u8 pic2_irq_interrupt_base ) {
    u8 pic1_data_save, pic2_data_save;

    pic1_data_save = ioport_readb( MASTER_PIC_DATA_PORT );
    pic2_data_save = ioport_readb( SLAVE_PIC_DATA_PORT );

    /* ICW1 */
    ioport_writeb( MASTER_PIC_CMD_PORT, PIC_ICW1_INIT | PIC_ICW1_NEED_ICW4 );
    ioport_writeb( SLAVE_PIC_CMD_PORT,  PIC_ICW1_INIT | PIC_ICW1_NEED_ICW4 );

    /* ICW2 */
    ioport_writeb( MASTER_PIC_DATA_PORT, pic1_irq_interrupt_base );
    ioport_writeb( SLAVE_PIC_DATA_PORT,  pic2_irq_interrupt_base );

    /* ICW3 */
    ioport_writeb( MASTER_PIC_DATA_PORT, 0x04 );   // IRQ2
    ioport_writeb( SLAVE_PIC_DATA_PORT,  0x02 );   // IRQ9

    /* ICW4 */
    ioport_writeb( MASTER_PIC_DATA_PORT, PIC_ICW4_8086_MODE );
    ioport_writeb( SLAVE_PIC_DATA_PORT,  PIC_ICW4_8086_MODE );

    ioport_writeb( MASTER_PIC_DATA_PORT, pic1_data_save );
    ioport_writeb( SLAVE_PIC_DATA_PORT,  pic2_data_save );
}
