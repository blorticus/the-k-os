#include <platform/x86_64/pic_8259.h>
#include <platform/x86_64/asm.h>
#include <sys/types.h>


pic_error pic_initialize( u8 irq_remap_first_interrupt ) {
    u8 pic1_data, pic2_data;
//    u8 irq_remap_slave_start;

    if (irq_remap_first_interrupt & 0x07)
        return PIC_ALIGNMENT_ERROR;

    // ASSERT: irq_remap_first_interrupt <= 248 (anything higher would raise PIC_ALIGNMENT ERROR above)
//    irq_remap_slave_start = irq_remap_first_interrupt + 8;

    // grab whatever is stored in the master and slave PIC data registers
    pic1_data = ioport_readb( PIC1_DATA_IO_PORT );
    pic2_data = ioport_readb( PIC2_DATA_IO_PORT );

    asm volatile( "movb $0x11, %%al\n\t"
                  "out %%al, $0x20\n\t"
                  "out %%al, $0xa0\n\t"
                  "movb $32, %%al\n\t"
                  "out %%al, $0x21\n\t"
                  "movb $40, %%al\n\t"
                  "out %%al, $0xa1\n\t"
                  "movb $4, %%al\n\t"
                  "out %%al, $0x21\n\t"
                  "movb $2, %%al\n\t"
                  "out %%al, $0xa1\n\t"
                  "movb $0x01, %%al\n\t"
                  "out %%al, $0x21\n\t"
                  "out %%al, $0xa1\n\t"
                  :
                  :
                  : "al" );

//    // begin init sequence on both master and slave PIC.  Since there are two
//    // PICs, cascading is in use.  All interrupts to 8925A assumed ISA, which
//    // are all Edge triggered
//    ioport_writeb( PIC1_COMMAND_IO_PORT, PIC_ICW1_BASE | PIC_ICW1_ICW4_NEEDED );
//    ioport_writeb( PIC2_COMMAND_IO_PORT, PIC_ICW1_BASE | PIC_ICW1_ICW4_NEEDED );
//
//    // ICW2 is the interrupt number for IRQ0 on the PIC
//    ioport_writeb( PIC1_DATA_IO_PORT, irq_remap_first_interrupt );
//    ioport_writeb( PIC2_DATA_IO_PORT, irq_remap_slave_start );
//
//    // Because cascading selected in ICW1, ICW3 is provided.  For the master, each
//    // bit corresponds to an IRQ used for a slave (i.e., bit 0 is IRQ0, bit1
//    // is IRQ1, bit2 is IRQ2, etc).  For the slave, the value is the absolute
//    // value of the IRQ used to communicte with the master.
//    ioport_writeb( PIC1_COMMAND_IO_PORT, 0x04 );    // that is, IRQ2 is a slave
//    ioport_writeb( PIC2_COMMAND_IO_PORT, 0x02 );
//
//    // Because ICW4 was requested in ICW1 (and is required for x86 architecture),
//    // declare x86 mode
//    ioport_writeb( PIC1_COMMAND_IO_PORT, PIC_ICW4_X86 );
//    ioport_writeb( PIC2_COMMAND_IO_PORT, PIC_ICW4_X86 );

    // restore whatever was stored in the master and slave PIC data registers
    ioport_writeb( PIC1_DATA_IO_PORT, pic1_data );
    ioport_writeb( PIC2_DATA_IO_PORT, pic2_data );

    return PIC_NO_ERROR;
}


pic_error pic_send_eoi( u8 irq_num ) {
    // IRQ >= 8 handled by slave, so must issue EOI (using PIC OCW2) to slave first then master
    if (irq_num >= 8)
        ioport_writeb( PIC2_COMMAND_IO_PORT, PIC_OCW2_NON_SPECIFIC_EOI );

    // all IRQs must send EOI to master
    ioport_writeb( PIC1_COMMAND_IO_PORT, PIC_OCW2_NON_SPECIFIC_EOI );

    return PIC_NO_ERROR;
}


pic_error pic_mask_all_irqs() {
    // OCW1 is a write to the PIC data port after initialization is completed.  Each bit
    // corresponds to an IRQ serviced by the PIC in question.  If the bit is set to 1,
    // the IRQ is masked (that is, the masked IRQ is ignored)
    ioport_writeb( PIC1_DATA_IO_PORT, 0xff );
    ioport_writeb( PIC2_DATA_IO_PORT, 0xff );

    return PIC_NO_ERROR;
}
