#include <lcheck.h>
#include <process/task.h>
#include <sys/types.h>
#include <util/kqueue.h>
#include <util/kcirc_list.h>
#include <util/kbit_field.h>


/* defined in process/task.c */
extern kqueue*      get_free_task_queue();
extern kcirc_list*  get_running_task_list();
extern kbit_field*  get_tasks_allocated();
extern TASK         get_active_task();
extern task*        get_tasks_array();
extern u32*         get_stacks_array();

/* stub */
void halt_os() { return; }

void method1() { return; }
void method2() { return; }

int main( void ) {
    lcheck_suite* s;
    task* task_array;
    u32*  stacks_array;
    TASK t1, t2, t3;
    u32 next_esp;
    
    s = create_suite( "task" );

    init_task_sys();
    task_array = get_tasks_array();
    stacks_array = get_stacks_array();

    fail_unless( s, get_active_task() == &task_array[0],        "After init_task_sys() active task is not the null task" );
    fail_unless( s, !kqueue_is_empty( get_free_task_queue() ),  "After init_task_sys() free_task_queue is empty" );
    fail_unless( s, kcirc_list_is_empty( get_running_task_list() ),     "After init_task_sys(), running_task_list is not empty" );

    t1 = task_create( method1 );

    fail_unless( s, t1 == &task_array[1],                               "First task_create() returns task 1" );
    fail_unless( s, t1->id == 1,                                        "First task_create() task id is not 1" );
    fail_unless( s, get_active_task() == &task_array[0],                "After first task_create() active task is not the null task" );
    fail_unless( s, !kcirc_list_is_empty( get_running_task_list() ),    "After first task_create(), running_task_list is empty" );

    next_esp = task_switch( 10 );
    fail_unless( s, next_esp == (u32)(&stacks_array[4096 - 15]),       "first task_switch() returns bottom of first stack array entry" );
    fail_unless( s, get_active_task() == &task_array[1],               "After first task_switch() active task is not task 1" );

    t2 = task_create( method2 );

    fail_unless( s, t2 == &task_array[2],                               "Second task_create() failed to return task 2" );
    fail_unless( s, t2->id == 2,                                        "Second task_create() task id is not 2" );
    fail_unless( s, get_active_task() == &task_array[1],                "After second task_create() active task is not task 1" );
    fail_unless( s, !kcirc_list_is_empty( get_running_task_list() ),    "After second task_create(), running_task_list is empty" );

    next_esp = task_switch( t1->uesp );
    fail_unless( s, next_esp == (u32)(&stacks_array[2 * 4096 - 15]),   "second task_switch() returns bottom of second stack array entry" );
    fail_unless( s, get_active_task() == &task_array[2],               "After second task_switch() active task is not task 2" );

    next_esp = task_switch( t2->uesp );
    fail_unless( s, next_esp == (u32)(&stacks_array[4096 - 15]),       "third task_switch() returns bottom of first stack array entry" );
    fail_unless( s, get_active_task() == &task_array[1],               "After third task_switch() active task is not task 1" );

    t3 = task_create( method2 );

    next_esp = task_switch( t1->uesp );
    fail_unless( s, next_esp == (u32)(&stacks_array[3 * 4096 - 15]),   "third task_switch() returns bottom of third stack array entry" );
    fail_unless( s, get_active_task() == &task_array[3],               "After third task_switch() active task is not task 3" );

    return conclude_suite( s );
}
