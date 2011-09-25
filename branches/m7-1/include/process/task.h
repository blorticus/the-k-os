#ifndef __TASK_H__
#define __TASK_H__

#include <sys/types.h>

typedef u32 tid_t;

 /*
  * DESCRIPTION:     A datastructure required for a task
  * ELEMENTS:        - uesp    : the last ESP before task was interrupted (or top of stack on task create)
  */
typedef struct task {
    tid_t id;
    u32 uesp;
} task;

typedef task* TASK;

/* The maximum number of allocated tasks for the system */
#define MAX_TASKS   8

void init_task_sys( void );
void task_release( TASK t );
TASK task_create( void (*task_start)(void) );
u32 task_switch( u32 previous_task_esp );


#endif
