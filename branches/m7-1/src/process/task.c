#include <process/task.h>
#include <util/kqueue.h>
#include <util/kcirc_list.h>
#include <util/kbit_field.h>
#include <video/kterm.h>
#ifndef TEST
#include <platform/ia-32/asm.h>
#else
extern void raise_int_128();
#endif

// stacks allocated to tasks; stacks[t - 1][4095] corresponds to start of stack for task with id (t)
// XXX: when this is working, convert to kmalloc with more refined stack management
u32 task_stack[MAX_TASKS][4096];

kqueue free_task_queue;
kcirc_list running_task_list;

kernel_common_ds_node _task_nodes[MAX_TASKS + 1];
task tasks[MAX_TASKS + 1];  // tasks[0] is the null task

kbit_field tasks_allocated;
u32 _tasks_allocated_bf[(int)(MAX_TASKS / 32) + 1];

TASK active_task;

#ifdef TEST
kqueue*     get_free_task_queue()       { return &free_task_queue;      }
kcirc_list* get_running_task_list()     { return &running_task_list;    }
kbit_field* get_tasks_allocated()       { return &tasks_allocated;      }
TASK        get_active_task()           { return active_task;           }
task*       get_tasks_array()           { return tasks;                 }
u32*        get_stacks_array()          { return (u32*)task_stack;      }
#endif

extern void halt_os();
extern void map_timer_for_multitasking();

void init_task_sys( void ) {
    int i;

    kqueue_init( &free_task_queue );
    kcirc_list_init( &running_task_list );

    _task_nodes[0].data = (void*)&tasks[0];
    tasks[0].id = 0;

    for (i = 1; i <= MAX_TASKS; i++) {
        _task_nodes[i].data = (void*)&tasks[i];
        tasks[i].id = i;
        kqueue_enqueue( &free_task_queue, &_task_nodes[i] );
    }

    kbit_field_init( &tasks_allocated, _tasks_allocated_bf, MAX_TASKS );

    active_task = &tasks[0];

    map_timer_for_multitasking();
}

void task_exit() {
    if (active_task != NULL)
        task_release( active_task->id );
}

void task_release( tid_t id ) {
    if (id > MAX_TASKS || id == 0)
        return;

    if (!kbit_field_is_set( &tasks_allocated, id ))
        return;

    kcirc_list_remove_node( &running_task_list, &_task_nodes[id] );

    if (kcirc_list_is_empty( &running_task_list ))
        halt_os();

    kqueue_enqueue( &free_task_queue, &_task_nodes[id] );

    kbit_field_clear( &tasks_allocated, id );

    if (active_task->id == id)
        raise_int_128();    // force task switch now
}

//TASK task_create( tid_t id, void (*task_start)(void) ) {
TASK task_create( void (*task_start)(void) ) {
    u32* stack;
    TASK t;
    u32 tid;

    if (kqueue_is_empty( &free_task_queue ))
        return NULL;

    t = (TASK)(kqueue_dequeue( &free_task_queue)->data);

    tid = t->id;

    stack = task_stack[tid];  // same as [tid - 1][4096]; first stack assignment decrements pointer first (to [tid - 1][4095])

    *--stack = 0x0202;      // eflags: required one bit and allow interrupts bit set
    *--stack = 0x08;        // cs
    *--stack = (u32)task_start;  // initial esp

    *--stack = 0; // edi
    *--stack = 0; // esi
    *--stack = 0; // ebp
    *--stack = 0; // esp pushed from pusha (not used initially)
    *--stack = 0; // ebx
    *--stack = 0; // edx
    *--stack = 0; // ecx
    *--stack = 0; // eax
    *--stack = 0x10; // ds
    *--stack = 0x10; // es
    *--stack = 0x10; // fs
    *--stack = 0x10; // gs

    t->uesp = (u32)stack;

    kbit_field_set( &tasks_allocated, tid );

    kcirc_list_add( &running_task_list, &_task_nodes[tid] );

    return t;
}


// XXX: Calls to this method must be atomic.  If this is called before a task
//      is created, the results are undefined
u32 task_switch( u32 previous_task_esp ) {
//  lock_scheduler();    <---- XXX: NEED TO IMPLEMENT THIS
    active_task->uesp = previous_task_esp;

    active_task = (TASK)(kcirc_list_advance( &running_task_list )->data);
//  unlock_scheduler();   <---- XXX: NEED TO IMPLEMENT THIS

    return active_task->uesp;
}
