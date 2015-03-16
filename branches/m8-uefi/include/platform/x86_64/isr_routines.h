#ifndef PLATFORM_ISR_ROUTINES
#define PLATFORM_ISR_ROUTINES

void isr_alignment_check( void );
void isr_bound_range( void );
void isr_breakpoint( void );
void isr_debug( void );
void isr_device_not_available( void );
void isr_divide_by_zero( void );
void isr_double_fault( void );
void isr_floating_point_exception_pending( void );
void isr_general_protection( void );
void isr_invalid_opcode( void );
void isr_invalid_tss( void );
void isr_machine_check( void );
void isr_non_maskable_interrupt( void );
void isr_overflow( void );
void isr_page_fault( void );
void isr_routine_does_not_exist( void );
void isr_security_exception( void );
void isr_segment_not_present( void );
void isr_simd_floating_point( void );
void isr_stack( void );
void isr_irq_pic_spurious( u8 interrupt, u8 irq );

#endif
