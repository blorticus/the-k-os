#include <process/task.h>
#include <util/kqueue.h>
#include <util/kcirc_list.h>
#include <util/kbit_field.h>

// stacks allocated to tasks; stacks[t - 1][4095] corresponds to start of stack for task with id (t)
// XXX: when this is working, convert to kmalloc with more refined stack management
u32 task_stack[MAX_TASKS][4096];

kqueue free_task_queue;
kcirc_list running_task_list;

kernel_common_ds_node _task_nodes[MAX_TASKS];
task tasks[MAX_TASKS];

kbit_field tasks_allocated;
u32 _tasks_allocated_bf[(int)(MAX_TASKS / 32) + 1];

TASK active_task;

extern void halt_os();

void init_task_sys( void ) {
    int i;

    kqueue_init( &free_task_queue );
    kcirc_list_init( &running_task_list );

    for (i = 0; i < MAX_TASKS; i++) {
        _task_nodes[i].data = (void*)&tasks[i];
        tasks[i].id = i + 1;
        kqueue_enqueue( &free_task_queue, &_task_nodes[i] );
    }

    kbit_field_init( &tasks_allocated, _tasks_allocated_bf, MAX_TASKS );

    active_task = NULL;
}

// XXX: why would I ever release based on a TASK rather than task id?
void task_release( TASK t ) {
    u32 toff = t->id - 1;

    if (toff >= MAX_TASKS)
        return;

    if (!kbit_field_is_set( &tasks_allocated, t->id ))
        return;

    kcirc_list_remove_node( &running_task_list, &_task_nodes[toff] );

    if (kcirc_list_is_empty( &running_task_list ))
        halt_os();

    kqueue_enqueue( &free_task_queue, &_task_nodes[toff] );

    kbit_field_clear( &tasks_allocated, t->id );

    if (active_task->id == t->id) {
        active_task = (TASK)(kcirc_list_peek_current( &running_task_list )->data);
        // XXX: Must launch scheduler here
    }
}

//TASK task_create( tid_t id, void (*task_start)(void) ) {
TASK task_create( void (*task_start)(void) ) {
    u32* stack;
    TASK t;
    u32 toff;

    if (kqueue_is_empty( &free_task_queue ))
        return NULL;

    t = (TASK)(kqueue_dequeue( &free_task_queue)->data);

    toff = t->id - 1;

    stack = task_stack[toff];  // same as &task_stack[id - 1][4096]; reference 4096 because stack grows down

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

    tasks[toff].uesp = (u32)stack;

    kbit_field_set( &tasks_allocated, t->id );

    kcirc_list_add( &running_task_list, &_task_nodes[toff] );

    if (active_task == NULL)
        active_task = t;

    return t;
}


// XXX: Calls to this method must be atomic.  If this is called before a task
//      is created, the results are undefined
u32 task_switch( u32 previous_task_esp ) {
    active_task->uesp = previous_task_esp;

    active_task = (TASK)(kcirc_list_advance( &running_task_list )->data);

    return active_task->uesp;
}
