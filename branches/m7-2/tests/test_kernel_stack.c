#include <lcheck.h>
#include <util/kernel_stack.h>
#include <sys/types.h>

u32 buf[15];


void test_stack_pop( lcheck_suite* s, KERNEL_STACK stack, u32 expected_value, u32 expected_size_after_pop, u32* base, char* testname ) {
    fmt_fail_unless( s, kernel_stack_peek( stack ) == expected_value,
                     "%s: peek does not yield expected value %d", testname, expected_value );

    fmt_fail_unless( s, kernel_stack_pop( stack ) == expected_value,
                     "%s: pop does not yield expected value %d", testname, expected_value );

    fmt_fail_unless( s, kernel_stack_size( stack ) == expected_size_after_pop,
                     "%s: after pop size is not expected value %d", testname, expected_size_after_pop );

    fmt_fail_unless( s, kernel_stack_limit( stack ) == (memptr)(base + expected_size_after_pop),
                     "%s: after pop limit is not %i from base", testname, expected_size_after_pop );
}


int main( void ) {
    lcheck_suite* s;

    struct kernel_stack ks;
    KERNEL_STACK stack = &ks;

    s = create_suite( "kernel_stack" );

    kernel_stack_init( stack, (memptr)buf );

    fail_unless( s, kernel_stack_size( stack ) == 0,
                 "Initialization, stack size is not zero" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)buf,
                 "Initialization, stack limit is not base ptr" );

    fail_unless( s, kernel_stack_peek( stack ) == 0,
                 "Initializtion, stack peek does not return 0" );

    fail_unless( s, kernel_stack_pop( stack ) == 0,
                 "Initialization, stack pop does not return 0" );

    fail_unless( s, kernel_stack_size( stack ) == 0,
                 "Initialization, after pop, stack size is not zero" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)buf,
                 "Initialization, after pop, stack limit is not base ptr" );

    kernel_stack_push( stack, 35 );

    fail_unless( s, kernel_stack_size( stack ) == 1,
                 "After First Push, stack size is not one" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 1),
                 "After First Push, stack limit is not base ptr plus one" );

    fail_unless( s, kernel_stack_peek( stack ) == 35,
                 "After First Push, stack peek does not return 35" );

    fail_unless( s, kernel_stack_pop( stack ) == 35,
                 "After First Push, stack pop does not return 35" );

    fail_unless( s, kernel_stack_size( stack ) == 0,
                 "After First Push, after pop, stack size is not zero" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)buf,
                 "After First Push, after pop, stack limit is not base ptr" );

    fail_unless( s, kernel_stack_peek( stack ) == 0,
                 "After First Push, after pop, stack peek does not return 0" );

    fail_unless( s, kernel_stack_pop( stack ) == 0,
                 "After First Push, after pop, stack pop does not return 0" );

    fail_unless( s, kernel_stack_size( stack ) == 0,
                 "After First Push, after pop, stack size is not zero" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)buf,
                 "After First Push, after pop, stack limit is not base ptr" );

    kernel_stack_push( stack, 79 );
    kernel_stack_push( stack, 13356 );
    kernel_stack_push( stack, 0xc0000000 );

    fail_unless( s, kernel_stack_size( stack ) == 3,
                 "Second Push Set, stack size is not three" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 3),
                 "Second Push Set, stack limit is not base ptr plus three" );

    fail_unless( s, kernel_stack_peek( stack ) == 0xc0000000,
                 "Second Push Set, top stack value peek not 0xc0000000" );

    fail_unless( s, kernel_stack_size( stack ) == 3,
                 "Second Push Set, after peek, stack size is not three" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 3),
                 "Second Push Set, after peek, stack limit is not base ptr plus three" );

    fail_unless( s, kernel_stack_pop( stack ) == 0xc0000000,
                 "Second Pish Set, first pop does not yield 0xc0000000" );

    fail_unless( s, kernel_stack_size( stack ) == 2,
                 "Second Push Set, after first pop, stack size is not two" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 2),
                 "Second Push Set, after first pop, stack limit is not base ptr plus two" );

    fail_unless( s, kernel_stack_peek( stack ) == 13356,
                 "Second Push Set, after first pop, peek does not return 13356" );

    kernel_stack_push( stack, 809 );

    fail_unless( s, kernel_stack_peek( stack ) == 809,
                 "Second Push Set, after first add'l push, peek does not return 809" );

    fail_unless( s, kernel_stack_size( stack ) == 3,
                 "Second Push Set, after first add'l push, stack size is not tnree" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 3),
                 "Second Push Set, after first add'l push, stack limit is not base ptr plus three" );

    kernel_stack_push( stack, 333 );
    kernel_stack_push( stack, 444 );

    fail_unless( s, kernel_stack_peek( stack ) == 444,
                 "Second Push Set, after second add'l pushes, peek does not return 444" );

    fail_unless( s, kernel_stack_size( stack ) == 5,
                 "Second Push Set, after second add'l pushes, stack size is not five" );

    fail_unless( s, kernel_stack_limit( stack ) == (memptr)((u32*)buf + 5),
                 "Second Push Set, after second add'l pushes, stack limit is not base ptr plus five" );

    // TEST SET 3
    test_stack_pop( s, stack, 444, 4, (u32*)buf, "TEST SET 3" );
    test_stack_pop( s, stack, 333, 3, (u32*)buf, "TEST SET 3" );
    test_stack_pop( s, stack, 809, 2, (u32*)buf, "TEST SET 3" );
    test_stack_pop( s, stack, 13356, 1, (u32*)buf, "TEST SET 3" );
    test_stack_pop( s, stack, 79, 0, (u32*)buf, "TEST SET 3" );
    test_stack_pop( s, stack, 0, 0, (u32*)buf, "TEST SET 3 (stack is empty before pop)" );

    // TEST SET 4
    kernel_stack_push_repeat( stack, 5, 10, 5 );

    test_stack_pop( s, stack, 30, 4, (u32*)buf, "TEST SET 4" );
    test_stack_pop( s, stack, 25, 3, (u32*)buf, "TEST SET 4" );
    test_stack_pop( s, stack, 20, 2, (u32*)buf, "TEST SET 4" );
    test_stack_pop( s, stack, 15, 1, (u32*)buf, "TEST SET 4" );
    test_stack_pop( s, stack, 10, 0, (u32*)buf, "TEST SET 4" );
    test_stack_pop( s, stack, 0, 0, (u32*)buf, "TEST SET 4 (stack is empty before pop)" );



    return conclude_suite( s );
}

