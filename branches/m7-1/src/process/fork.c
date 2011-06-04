/**
 * This is nowhere near a real fork().  I just needed a name for process initiation, so here it is
 */

#include <sys/types.h>
#include <process/scheduler.h>

typedef u32 pid_t;

/* from the kernel, create a new process, providing the process a pointer to a function, 'pfunc'.  kfork()
   returns the process id of the created process.  When 'pfunc' returns, the process terminates. */
pid_t kfork( void (*pfunc)(void) ) {
    return schedule_thread_for_function( pfunc );
}
