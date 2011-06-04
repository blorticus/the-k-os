#include <process/scheduler.h>
#include <platform/ia-32/asm.h>
#include <util/kcommonds.h>
#include <util/kqueue.h>
#include <util/kcirc_list.h>

// // Keep two data structures: a thread pool queue of available (unallocated) threads; and a circular list of
// // active (allocated) threads.  When a thread is scheduled, pass it a function (the code for
// // the thread to run).  Remove a thread pool member, and initiatize the thread object to which its
// // node points.  When the thread function returns, lock the scheduler (by disabling interrupts), then
// // move the thread node from the active list to the thread pool queue.
// kernel_common_ds_node tnode[THREAD_TBL_SIZE];
// thread thrd[THREAD_TBL_SIZE];
// kqueue tpool;
// kcirc_list tactive;
// THREAD tlast;
// 

// temporary stacks allocated to processes.
// XXX: when this is working, convert to kmalloc with more refined stack management
u32 stacks[20 * 4096];

// static inline void lock_scheduler()   { disable_interrupts(); }
// static inline void unlock_scheduler() { enable_interrupts (); }
// 
// void relinquish( void ) {
//     raise_interrupt( 0x80 );
// }
// 
// void initialize_scheduler( void ) {
//     int i;
// 
//     kqueue_init( &tpoolq );
//     kcirc_list_init( &tactivel );
// 
//     for (i = 0; i < THREAD_TBL_SIZE; i++) {
//         thrd[i].id = i + 1;
//         (THREAD)(tnode[i].data) = &thrd[i];
//         kqueue_enqueue( &tpoolq, tnode[i] );
//     }
// }
// 
// // this method handles initiation of a function thread and its
// // teardown.  At this point, returns 0 on success or
// // 1 if the thread could not be allocated
// static int handle_function_thread( void (*pfunc)(void), pid_t pid ) {
//     return 0;
// }
// 
// 
// pid_t schedule_thread_for_function( void (*pfunc)(void) ) {
//     kernel_common_ds_node* n;
// 
//     lock_scheduler();
//     if ((n = kqueue_dequeue( &tpoolq )) == NULL) { // thread pool is empty
//         unlock_scheduler();
//         return 0;
//     }
// 
//     kcirc_list_add( &tactive, n );
//     unlock_scheduler();
// 
//     return ((THREAD)(n->data))->id;
// }
// 
// 
// /**
//  * DESCRIPTION      : Create a thread of execution, with a clean stack and general registers in an undefined state.
//  *                    The execution start point is given by 'start'
//  * RETURNS          : 0 on success, non-zero on failure
//  **/
thread _t1, _t2;;
THREAD t1 = &_t1;
THREAD t2 = &_t2;
u8 active_thread = 1;  // 1 or 2
 
int thread_create( void (*start)(void) ) {
    THREAD newt;
    u32 *stack;  // stacks grown *down* so use this pointer to invert

    newt = active_thread == 1 ? t2 : t1;

    newt->edi = newt->esi = newt->ebx = newt->edx = newt->ecx = newt->eax = 0;
    newt->eip = (u32)start;

    stack = &stacks[0] + 4095;

    newt->ebp = newt->esp = (u32)stack;
    newt->eflags = 0x02;  // all flags, but the require '1' in the second bit slot is one

    return active_thread == 1 ? 2 : 1;
}


void thread_switch() {
    THREAD from, to;

    if (active_thread == 1) {
        from = t1;
        to   = t2;
    }
    else {
        from = t2;
        to   = t1;
    }

//    asm( "pop %%eax" : : : "eax" );  // address of this function
    asm( "pop %%eax" : : : "eax" );  // gs
    asm( "pop %%eax" : : : "eax" );  // fs
    asm( "pop %%eax" : : : "eax" );  // es
    asm( "pop %%eax" : : : "eax" );  // ds
    asm( "pop %0" : "=m"(from->edi) : );
    asm( "pop %0" : "=m"(from->esi) : );
    asm( "pop %0" : "=m"(from->ebp) : );
    asm( "pop %0" : "=m"(from->esp) : );
    asm( "pop %0" : "=m"(from->ebx) : );
    asm( "pop %0" : "=m"(from->edx) : );
    asm( "pop %0" : "=m"(from->ecx) : );
    asm( "pop %0" : "=m"(from->eax) : );
//    asm( "pop %%eax" : : : "eax" );  // irq number
//    asm( "pop %%eax" : : : "eax" );  // error code
    asm( "pop %0" : "=m"(from->eip) : );
    asm( "pop %%eax" : : : "eax" );  // cs
    asm( "pop %0" : "=m"(from->eflags) : );

    asm( "mov %0, %%eax " : : "m"(to->eax) : "eax" );
    asm( "mov %0, %%ebx " : : "m"(to->ebx) : "ebx" );
    asm( "mov %0, %%ecx " : : "m"(to->ecx) : "ecx" );
    asm( "mov %0, %%edx " : : "m"(to->edx) : "edx" );
    asm( "mov %0, %%edi " : : "m"(to->edi) : "edi" );

    // repoint to new stack
    asm( "mov %0, %%esp"    : : "m"(to->esp) : "esp" );
//    asm( "mov %%esp, %%ebp" : : );  // ebp not allowed in clobber; don't know why

    // IRET pops EFLAGS, CS and WIP (in that order) from the stack
    asm( "push %0"      : : "m"(to->eflags) : "memory" );
    asm( "push $8"      : :                 : "memory" );  // CS is 0x08
    asm( "push %0"      : : "m"(to->eip)    : "memory" );

    // don't need to unlock scheduler (STI) -- and create a race condition
    // between STI and IRET -- because IRET restores EFLAGS, which must have
    // interrupt flag set, or we could never have arrived here for that task
    asm( "iret" );
}


void thread_exit() {
    
}



// /**
//  * This method requires certain pre-conditions about the state of the stack.
//  * In particular, it assumes that this function is being called from an
//  * interrupt handler.  It assumes that a 'jmp' instruction got us here (so
//  * there is no way to return from this function, which is fine.  It will
//  * load the stack so that it performs a 'iret' to the next thread scheduled,
//  * which of course means that there MUST always be at least one thread in
//  * the run list).  It assumes the stack contains the following:
//  *   eflags
//  *   cs
//  *   eip
//  *   error code
//  *   irq number
//  *   eax
//  *   ecx
//  *   edx
//  *   ebx
//  *   esp
//  *   ebp
//  *   esi
//  *   edi
//  *   ds
//  *   es
//  *   fs
//  *   gs
//  * Notice that the elements 5 .. 13 are result of 'pusha'.
//  * This function will remove all of those elements from the stack, restore
//  * specific register and flag values, switch stacks, load that stack with
//  * appropriate information to change to another task, then 'iret'
//  */
// void run_scheduler_from_interrupt( void ) {
//     kernel_common_ds_node* n;
// 
//     // pre:
//     //  - when scheduler is invoked, it is from interrupt
//     //  - scheduler must have pointer to last running thread object
//     //  - because scheduler invoked from interrupt, on iret, eflags is restored, and
//     //    cli must be in force (or interrupt would not have fired from that thread)
//     //    so unlock scheduler is implicit at the end on an iret
//     //  1. grab eip off of stack
//     //  2. populate eip into thread object
//     // flow:
//     //  1. grab next thread in run list
//     //  2. populate registers from thread object
//     //  3. push last instruction ptr to queue
//     //  4. perform iret
//     lock_scheduler();
// 
//     // tlast points to currently executing thread object
//     // pop the segment registers because we don't use segmentation
//     // so the first four are essentially "ignores"
//     asm( "pop %%eax" : : : "eax" );  // gs
//     asm( "pop %%eax" : : : "eax" );  // fs
//     asm( "pop %%eax" : : : "eax" );  // es
//     asm( "pop %%eax" : : : "eax" );  // ds
//     asm( "pop %0" : : "=m"(tlast->edi) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->esi) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->ebp) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->esp) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->ebx) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->edx) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->ecx) : "memory" );
//     asm( "pop %0" : : "=m"(tlast->eax) : "memory" );
//     asm( "pop %%eax" : : : "eax" );  // irq number
//     asm( "pop %%eax" : : : "eax" );  // error code
//     asm( "pop %0" : : "=m"(tlast->eip) : "memory" );
//     asm( "pop %%eax" : : : "eax" );  // cs
//     asm( "pop %0" : : "=m"(tlast->eflags) : "memory" );
// 
//     n = kcirc_list_advance( &tactive );
//     tlast = (THREAD)(n->data);
// 
//     asm( "mv %0, %%eax" : "=m"(tlast->eax) : : "eax" );
//     asm( "mv %0, %%ebx" : "=m"(tlast->ebx) : : "ebx" );
//     asm( "mv %0, %%ecx" : "=m"(tlast->ecx) : : "ecx" );
//     asm( "mv %0, %%edx" : "=m"(tlast->edx) : : "edx" );
//     asm( "mv %0, %%edi" : "=m"(tlast->edi) : : "edi" );
// 
//     // repoint to new stack
//     asm( "mov %0, %%ebp" : "=m"(tlast->ebp) : : "ebp" );
//     asm( "mov %0, %%esp" : "=m"(tlast->esp) : : "esp" );
// 
//     // IRET pops EIP, CS and EFLAGS (in that order) from the stack
//     asm( "push %0"      : "=m"(tlast->eip)    : : "memory" );
//     asm( "push $8"      :                     : : "memory" );  // CS is 0x08
//     asm( "push %0"      : "=m"(tlast->eflags) : : "memory" );
// 
//     // don't need to unlock scheduler (STI) -- and create a race condition
//     // between STI and IRET -- because IRET restores EFLAGS, which must have
//     // interrupt flag set, or we could never have arrived here for that task
//     asm( "iret" );
// }
