#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <sys/types.h>

typedef struct thread {
    u32 id;
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
    u32 esi;
    u32 edi;
    u32 eflags;
    u32 esp;
    u32 eip;
    u32 ebp;
}__attribute__((packed)) thread;

typedef thread* THREAD;

typedef struct task {
    u32 esp0;
    u32 esp3;
} task;

typedef u32 pid_t;

/**
 * define the maximum number of simultaneous running threads.  The thread->id is equal to
 * the table entry plus 1
 */
#define THREAD_TBL_SIZE     256


/**
 * DESCRIPTION  : initialize the thread scheduler
 * RETURN       : -
 * RE-ENTRANT   : no
 */
void initialize_scheduler( void );

/**
 * DESCRIPTION  : schedule a thread based on a function 'pfunc'
 * RETURN       : pid for new process or zero if the process creation failed
 * RE-ENTRANT   : yes
 */
pid_t schedule_thread_for_function( void (*pfunc)(void) );

#endif
