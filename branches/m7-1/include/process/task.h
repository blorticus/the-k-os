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
#define MAX_TASKS   16


#endif
