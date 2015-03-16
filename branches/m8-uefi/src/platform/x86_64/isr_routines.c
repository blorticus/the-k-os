#include <kerror.h>
#include <video/text_terminal.h>
#include <platform/x86_64/isr_routines.h>
#include <platform/x86_64/pic_8259.h>

void isr_alignment_check( void ) {
    kerror( L"Alignment Check Exception: #AC (17)\n" );
    for ( ;; ) ;
}

void isr_bound_range( void ) {
    kerror( L"Bound Range Exception: #BR (05)\n" );
    for ( ;; ) ;
}

void isr_breakpoint( void ) {
    kerror( L"Breakpoint Exception: #BP (03)\n" );
    for ( ;; ) ;
}

void isr_debug( void ) {
    kerror( L"Debug Exception: #DB (01)\n" );
    for ( ;; ) ;
}

void isr_device_not_available( void ) {
    kerror( L"Device Not Available Exception: #NM (07)\n" );
    for ( ;; ) ;
}

void isr_divide_by_zero( void ) {
    kerror( L"Divide By Zero Exception: #DE (00)\n" );
    for ( ;; ) ;
}

void isr_double_fault( void ) {
    kerror( L"Double Fault Exception: #DF (08)\n" );
    for ( ;; ) ;
}

void isr_floating_point_exception_pending( void ) {
    kerror( L"Floating Point Exception Pending Exception: #MF (17)\n" );
    for ( ;; ) ;
}

void isr_general_protection( void ) {
    kerror( L"General Protection Exception: #GP (13)\n" );
    for ( ;; ) ;
}

void isr_invalid_opcode( void ) {
    kerror( L"Invalid Opcode Exception: #UD (06)\n" );
    for ( ;; ) ;
}

void isr_invalid_tss( void ) {
    kerror( L"TSS Exception: #TS (10)\n" );
    for ( ;; ) ;
}

void isr_machine_check( void ) {
    kerror( L"Machine Check Exception: #MC (18)\n" );
    for ( ;; ) ;
}

void isr_non_maskable_interrupt( void ) {
    kerror( L"NMI Exception: NMI (02)\n" );
    for ( ;; ) ;
}

void isr_overflow( void ) {
    kerror( L"Overflow Exception: #OF (04)\n" );
    for ( ;; ) ;
}

void isr_page_fault( void ) {
    kerror( L"Page Fault Exception: #PF (14)\n" );
    for ( ;; ) ;
}

void isr_security_exception( void ) {
    kerror( L"Security Exception: #SX (30)\n" );
    for ( ;; ) ;
}

void isr_segment_not_present( void ) {
    kerror( L"Segment Not Present Exception: #NP (11)\n" );
    for ( ;; ) ;
}

void isr_simd_floating_point( void ) {
    kerror( L"SIMD Floating Point Exception: #XF (19)\n" );
    for ( ;; ) ;
}

void isr_stack( void ) {
    kerror( L"Stack Exception: #SS (12)\n" );
    for ( ;; ) ;
}


void isr_irq_pic_spurious( u8 interrupt, u8 irq ) {
    pic_send_eoi( irq );
    kerror_silly( interrupt, irq );
}

