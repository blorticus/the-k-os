#include <sys/types.h>
#include <video/kterm.h>
#include <platform/ia-32/interrupts.h>
#include <platform/ia-32/gdt.h>


/**
 *
 * On the IA-32 platform, an interrupt is an assertion to the processor that it
 * should stop its current thread of execution and switch to pre-defined code
 * based on the interrupt number.  The IA-32 platform supports 256 interrupts
 * numbered 0 .. 255, inclusive.  Interrupts arise from three sources:
 * Interrupt Requests (IRQs), processor exceptions, and a software call to the
 * 'int' instruction.  Interrupt numbers are called "vectors".
 *
 * An IRQ is asserted by a piece of hardware connected to the system bus.  There
 * are two possible ways that these IRQs are handled: either by a PIC or by an
 * APIC.  Currently, this OS uses the PIC method.  The
 * peripherals are connected via the bus to two Programmable Interrupt
 * Controllers (PICs), referred to as the "master" PIC and the "slave" PIC.  A
 * system supports up to 16 IRQs, numbered 0 .. 15, inclusive.  Some IRQ
 * numbers are reserved for specific services (e.g., IRQ1 is typically the
 * keyboard), while others are unassigned.  Via plug-and-play, most modern
 * peripherals negotiate an appropriate IRQ via the BIOS on system startup.
 * When a peripheral asserts an IRQ, it is intercepted by the master PIC if the
 * IRQ is 0 .. 7 or the slave PIC if the IRQ is 8 .. 15.  The PIC transforms
 * the IRQ into an interrupt vector, and asserts that interrupt on the
 * processor.  The interrupt vector to which IRQs map can be changed on the PIC
 * through software calls.
 *
 * Exceptions are pre-defined for the processor and are asserted when an instruction
 * is executed that the processor cannot or will not allow.  For example,
 * interrupt 0 is caused when the processor is asked to divide a value by the
 * number zero, which leads to an undefined result.  The processor cannot perform
 * the mathematical operation, because the result is essentially nonsense.  Other
 * exceptions are raised because of system protections.  For example, memory can
 * be protected such that certain tasks cannot access certain parts of memory.  A
 * General Protection Fault is asserted if a task tries to access memory which it
 * is not allowed to access.
 *
 * Software interrupts (or "soft" interrupts) are asserted when the 'int' instruction
 * is called.  Any interrupt vector can be asserted this way.  However, there are
 * privilege level protections to protect system interrupts.  For example, it is not
 * generally desirable for a user-level task (operating at privilege level 3) to
 * call the clock interrupt (IRQ 0, which is mapped via the PIC to some interrupt
 * vector), particularly if this interrupt is used to control task switching.
 *
 * Most hardware interrupts are maskable.  If a specific flag in EFLAGS is set to zero, then
 * the processor inhibits delivery of interrupt requests from the PICs.  Normally,
 * this flag is set to zero using the 'cli' (clear interrupt) instruction and set to one
 * (thus re-enabling delivery of interrupt requests) using the instruction 'sti' (set
 * interrupt).  A separate assertion pin on the processor signals the receipt of a
 * non-maskable interrupt.  This interrupt will fire even when normal hardware interrupts
 * are masked.  Masking interrupts affects hardware generated interrupts, sourced from
 * IRQs, but not exceptions and soft interrupts.
 *
 * There are 32 pre-defined exceptions, corresponding to interrupts 0 .. 31,
 * inclusive.  By default, the BIOS commands the PICs to translate IRQs 0 .. 15
 * to interrupts 0 .. 15, inclusive.  This obviously doesn't work, since this collides
 * with the exceptions, so it is prudent to remap the IRQ -> interrupt translations
 * after the system starts up.  This OS remaps the translation so that IRQs 0 .. 15
 * map to 32 .. 47, inclusive.
 *
 * Each vector is mapped in a system table called the Interrupt Descriptor Table.
 * Each table entry corresponds to an interrupt vector and provides an interrupt
 * descriptor for that vector.  This descriptor provides a pointer to code which
 * the processor runs when the interrupt is received.  This OS refers to interrupt
 * handlers as Interrupt Service Routines (ISRs).  There are actually three types
 * of descriptors supported: task gates, interrupt gates, and trap gates.  Task gates
 * are used when hardware task switching (via task segmenet selectors -- TSS's) is
 * used.  This OS does not use hardware task switching, so we won't consider task
 * gates further.  Interrupt gates and trap gates work exactly the same way, except
 * on a call to an interrupt gate, the EFLAGS IF flag is cleared, which means that
 * no other interrupt can fire until IF is reset.  A call through a trap gate does
 * not change IF.  This OS uses interrupt gates for all exception handling.
 *
 * When an interrupt fires, the processor configures the stack in a particular way.
 * The arrangement of the stack depends on the gate selector privilege level (DPL)
 * and the privilege level of code executing at the time of the interrupt (CPL).
 * if the CPL is the same as the DPL (e.g., privilege level 0 code is executing and
 * interrupt fires where the gate descriptor DPL is also 0), then the processor
 * copies EFLAGS, CS, EIP and the error code (if one is supposed to be generated).
 * If the CPL is numerically greater than the DPL (e.g., privilege level 3 is
 * executing and interrupt fires where the gate descriptor DPL is 0), then the
 * processor changes stacks, and copies SS, ESP, EFLAGS, CS, EIP and the error
 * code (if one is supposed to be generated).  Notice the stack change.  How does
 * the processor know to which stack it should change?  It will select SS and
 * ESP are retrieved from the currently executing hardware task segment.  This
 * OS does not use hardware task switching, but it must define one task segment
 * selector for each processor.  This TSS contains SS and ESP for each privilege
 * level that can be used by interrupt handlers.  This OS only uses privilege
 * levels 3 and 0, so only the SS/ESP for privilege level 0 needs to be defined.
 * If the CPL is numerically lower than DPL (e.g., privilege level 0 code is
 * executing and an interrupt fires where the gate descriptor DPL is 3), the
 * #GP is asserted.  This check is only made for soft interrupts.
 *
 * On the completion of an interrupt call, the ISR must call 'iret'.  This
 * pops the registers written to the stack by the processor (described above)
 * back to their registers, and in the case where a privilege level change
 * occurred, the original stack is restored (that's why SS and ESP are stored
 * on the switched-to stack).  Notice that EFLAGS is restored.  IF (in the
 * EFLAGS) *must* be set.  If it were not, then the interrupt could never have
 * fired.  So, if IF is cleared (as is the case when an exception gate is
 * used), there is no need to invoke 'sti' before 'iret'.  That will happen
 * implicitly when EFLAGS is restored.
 *
 * The base descriptor (for vector 0) should be aligned in linear memory
 * on an 8 byte boundary (i.e., the base address for the descriptor should
 * end with 1000).  This maximizes caching performance on the processor
 * when entries are accessed.  The descriptors must be an array of eight
 * byte values with the following structure:
 *
 * +--------+--------+--------+--------+
 * | Offset Low      | Code Seg Sel    |
 * +--------+--------+--------+--------+
 * |00000000|PD 0S11I| Offset High     | 
 * +--------+--------+-----------------+
 *
 * Offset is the address of the interrupt service routine.  The Code Segment
 * Selector identifies the GDT entry for code segment containing the offset.
 * This OS uses GDT entry 0x08 for the privilege level 0 code segment.  The
 * flags are:
 *  - P = present.  Set to 1 for an active descriptor or 0 for an inactive
 *        descriptor.  An ISR will not fire for an interrupt if P = 0.
 *  - D = DPL.  The privilege level to which the processor changes when the
 *        ISR executes.
 *  - S = Size.  If set to 0, then this is a 16-bit gate (so only offset low
 *        is used).  If set to 1, then this is a 32-bit gate (and both offset
 *        low and offset high are used).
 *  - I = Interrupt gate.  If set to 1, this is an interrupt gate.  If set to
 *        0, this is a trap gate.  The structure of a task gate is quite
 *        different, and is not described here.
 *
 * When the IDT entry array is created, the processor must be notified of its
 * location in linear memory.  This location is stored in the IDT register.
 * A value is loaded into this register using 'lidt'.  The data structure in
 * the register is:
 *
 * 0                16                               48
 * +----------------+--------------------------------+
 * | IDT Size Limit | IDT Base Addr                  |  
 * +--------------------------------+----------------+
 *
 * The Base Addr is the linear address of the first IDT array entry.  The Size
 * Limit is the size of the table in bytes minus 1.
 *
 * SEE:             http://www.acm.uiuc.edu/sigops/roll_your_own/i386/idt.html
 *                  http://en.wikipedia.org/wiki/Interrupt_request
 *                  http://www.isd.mel.nist.gov/projects/rtlinux/rtutorial-2.0/doc/ex05_isr.htm
 *                  http://en.wikipedia.org/wiki/Intel_8259
 **/


#define IDT_FLAG_PRESENT            0x80  // 1000 0000
#define IDT_FLAG_DPL_0              0x00  // 0000 0000
#define IDT_FLAG_DPL_1              0x20  // 0010 0000
#define IDT_FLAG_DPL_2              0x40  // 0100 0000
#define IDT_FLAG_DPL_3              0x60  // 0110 0000
#define IDT_FLAG_32BIT_SIZE         0x08  // 0000 1000
#define IDT_FLAG_16BIT_SIZE         0x00  // 0000 0000
#define IDT_IS_TRAP_GATE            0x05  // 0000 0101
#define IDT_IS_INTERRUPT_GATE       0x06  // 0000 0110
#define IDT_IS_TASK_GATE            0x07  // 0000 0111


/**
 *
 * DESCRIPTION:     A single entry in the IDT exactly as it is arranged in memory
 * ELEMENTS:        base_low_word    = the bottom half of the ISR address
 *                  gdt_selector     = the GDT code segment entry through which addressing should be mapped
 *                  zeroed_flags     = in the ia-32, several tables use the same struture as the IDT.  This is a flags field, but for
 *                                     the IDT table, it is all zeroes for all entries
 *                  descriptor_flags = flags for the descriptor, including:
 *                                      - 1   : present bit (if 0, raise exception on interrupt invocation)
 *                                      - 2,3 : privilege level ("ring")
 *                                      - 4   : must be 0
 *                                      - 5-8 : gate type: Task Gate (0x5), Interrupt Gate (32-bit = 0xE), or Trap Gate (32-bit = 0xF)
 *                  base_word_high   = the top half o the ISR address
 * NOTES:           MUST be packed, which prevents the compiler from aligning the struct on processor word boundaries.
 *                  Offset in Target Segment is the memory location of the handler (in the GDT Selector Segment) for the
 *                    exception handler of the referenced ISR slot.
 *                  An interrupt gate disables interrupts before execution of the handler and trap gates do not.
 *
 */ 
typedef struct idt_descriptor {
    u16 target_seg_offset_low;
    u16 gdt_code_selector;
    u8  zeroes;
    u8  type_and_flags;
    u16 target_seg_offset_high;
} __attribute__((packed)) idt_descriptor;


/**
 *
 * DESCRIPTION:     The load IDT asm instruction (lidt) is provided a pointer to a data structure that, itself, points to the actual
 *                  table.  This is that pointing structure
 * ELEMENTS:        limit       = the size of the table in bytes
 *                  base        = the physical memory address of the start of the table
 * NOTES:           MUST be packed, which prevents the compiler from aligning the struct on processor word boundaries
 *
 **/ 
typedef struct idt_register {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_register;
 

idt_descriptor  idt[256];
idt_register    idt_register_value;

/* This exists in 'start.asm', and is used to load our IDT */
//extern void idt_load();
extern void isr_do_nothing( void );

static void clear_all_isrs( void ) {
    int i;

    u16 do_nothing_low  = (u16)(u32)(isr_do_nothing);
    u16 do_nothing_high = (u16)((u32)(isr_do_nothing) >> 16);

    for (i = 0; i < 256; i++) {
        idt[i].gdt_code_selector      = GDT_DPL_0_CODE_SEGMENT_OFFSET;
        idt[i].zeroes                 = 0;
        idt[i].type_and_flags         = (u8)(IDT_FLAG_PRESENT | IDT_FLAG_DPL_0 | IDT_FLAG_32BIT_SIZE | IDT_IS_INTERRUPT_GATE);
        idt[i].target_seg_offset_low  = do_nothing_low;
        idt[i].target_seg_offset_high = do_nothing_high;
    }
}


/* Create an entry in the IDT */
void set_idt_gate_descriptor( u8 isr_num, u32 addr, u16 gdt_selector, u8 flags)
{
    idt[isr_num].target_seg_offset_low  = (u16)addr;
    idt[isr_num].target_seg_offset_high = (u16)(addr >> 16);

    idt[isr_num].gdt_code_selector  = gdt_selector;
    idt[isr_num].zeroes             = 0x00;
    idt[isr_num].type_and_flags     = flags;
}

static inline void load_idt( idt_register* reg ) {
    __asm__( "lidt (%0)" : : "p"(reg) );
}

/* Installs the IDT */
void install_idt()
{
    idt_register_value.limit = (sizeof (idt)) - 1;
    idt_register_value.base  = (u32)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    clear_all_isrs();

    load_idt( &idt_register_value );
}


extern void interrupt_00( void );
extern void interrupt_01( void );
extern void interrupt_02( void );
extern void interrupt_03( void );
extern void interrupt_04( void );
extern void interrupt_05( void );
extern void interrupt_06( void );
extern void interrupt_07( void );
extern void interrupt_08( void );
extern void interrupt_09( void );
extern void interrupt_10( void );
extern void interrupt_11( void );
extern void interrupt_12( void );
extern void interrupt_13( void );
extern void interrupt_14( void );
extern void interrupt_15( void );
extern void interrupt_16( void );
extern void interrupt_17( void );
extern void interrupt_18( void );
extern void interrupt_19( void );
extern void interrupt_20( void );
extern void interrupt_21( void );
extern void interrupt_22( void );
extern void interrupt_23( void );
extern void interrupt_24( void );
extern void interrupt_25( void );
extern void interrupt_26( void );
extern void interrupt_27( void );
extern void interrupt_28( void );
extern void interrupt_29( void );
extern void interrupt_30( void );
extern void interrupt_31( void );

extern void interrupt_128( void );

typedef void(*isr_handler_routine)(struct regs *r);


isr_handler_routine isr_routines[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0
};

inline void isr_set_handler( u8 isr_number, isr_handler_routine handler ) {
    isr_routines[isr_number] = handler;
}


#define STD_EXCEPTION_FLAGS   ((u8)(IDT_FLAG_PRESENT | IDT_FLAG_DPL_0 | IDT_FLAG_32BIT_SIZE | IDT_IS_INTERRUPT_GATE))

void isrs_install()
{
    set_idt_gate_descriptor( 0,  (u32)interrupt_00, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 1,  (u32)interrupt_01, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 2,  (u32)interrupt_02, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 3,  (u32)interrupt_03, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 4,  (u32)interrupt_04, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 5,  (u32)interrupt_05, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 6,  (u32)interrupt_06, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 7,  (u32)interrupt_07, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 8,  (u32)interrupt_08, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 9,  (u32)interrupt_09, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 10, (u32)interrupt_10, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 11, (u32)interrupt_11, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 12, (u32)interrupt_12, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 13, (u32)interrupt_13, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 14, (u32)interrupt_14, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 15, (u32)interrupt_15, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 16, (u32)interrupt_16, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 17, (u32)interrupt_17, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 18, (u32)interrupt_18, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 19, (u32)interrupt_19, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 20, (u32)interrupt_20, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 21, (u32)interrupt_21, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 22, (u32)interrupt_22, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 23, (u32)interrupt_23, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 24, (u32)interrupt_24, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 25, (u32)interrupt_25, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 26, (u32)interrupt_26, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 27, (u32)interrupt_27, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 28, (u32)interrupt_28, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 29, (u32)interrupt_29, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 30, (u32)interrupt_30, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
    set_idt_gate_descriptor( 31, (u32)interrupt_31, GDT_DPL_0_CODE_SEGMENT_OFFSET, STD_EXCEPTION_FLAGS );
}


/**
 * This is a simple string array. It contains the message that corresponds to
 * each and every exception. We get the correct message by accessing like:
 *   exception_message[interrupt_number]
 **/

char *exception_messages[] =
{
    "Division Error",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "No Match Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",

    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "Floating Point Unit Error",
    "Alignment Check Exception",
    "Machine Check Exception",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved"
};


KTERM_WINDOW kterm_win = NULL;

void fault_handler_set_kterm_window( KTERM_WINDOW w ) {
    kterm_win = w;
}


void exception_handler( struct regs *r ) {
    isr_handler_routine h;

    if (r->int_no < 32)
    {
        if (kterm_win != NULL)
            kterm_window_printf( kterm_win, "Exception: %s [0x%x]\n", exception_messages[r->int_no], r->err_code );
        for ( ;; );
    }
    else if ((h = isr_routines[r->int_no])) {
        h( r );
    }
}

void isr_test( struct regs *r ) {
    kterm_window_printf( kterm_win, "TEST ISR\n" );
}

