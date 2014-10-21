#include <lcheck.h>
#include <apps/kosh/kosh_cmd.h>
#include <sys/types.h>


void* kmalloc( size_t bytes ) {
    return __malloc( bytes );
}


char* cmd_called = NULL;
char32_t** called_argv = NULL;
int called_argc = -1;

kosh_error cmd1_proc( char32_t** argv, int argc, char32_t* error ) {
    cmd_called  = "cmd1";
    called_argv = argv;
    called_argc = argc;
    return KOSH_OK;
}


kosh_error cmd12_proc( char32_t** argv, int argc, char32_t* error ) {
    cmd_called  = "cmd12";
    called_argv = argv;
    called_argc = argc;
    return KOSH_OK;
}


kosh_error cmd2_proc( char32_t** argv, int argc, char32_t* error ) {
    cmd_called  = "cmd2";
    called_argv = argv;
    called_argc = argc;
    return KOSH_OK;
}



int main( void ) {
    lcheck_suite* s;
    kosh_cmd_list l1;
    char32_t* error = 0;

    s = create_suite( "apps/kosh/kosh_cmd" );

    fail_unless( s, kosh_init_cmd_structure( &l1 ) == KOSH_OK, "kosh_init_cmd_structure does not return KOSH_OK" );

    fail_unless( s, kosh_register_cmd( &l1, (const char32_t*)L"cmd1", cmd1_proc ) == KOSH_OK, "kosh_register_cmd for cmd1_proc does not return KOSH_OK" );
    fail_unless( s, kosh_register_cmd( &l1, (const char32_t*)L"cmd12", cmd12_proc ) == KOSH_OK, "kosh_register_cmd for cmd12_proc does not return KOSH_OK" );
    fail_unless( s, kosh_register_cmd( &l1, (const char32_t*)L"cmd2", cmd2_proc ) == KOSH_OK, "kosh_register_cmd for cmd2_proc does not return KOSH_OK" );

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"", error ) == KOSH_EMPTY_COMMAND, "kosh_execute_cmd for '' does not return KOSH_EMPTY_COMMAND" );
    fail_unless( s, cmd_called == NULL, "kosh_execute_cmd for '' cmd_called not NULL" );
    fail_unless( s, called_argv == NULL, "kosh_execute_cmd for '' called_argv not NULL" );
    fail_unless( s, called_argc == -1, "kosh_execute_cmd for '' called_argc not -1" );

    cmd_called = NULL; called_argv = NULL; called_argc = -1;

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"cmd", error ) == KOSH_NO_SUCH_COMMAND, "kosh_execute_cmd for 'cmd' does not return KOSH_NO_SUCH_COMMAND" );
    fail_unless( s, cmd_called == NULL, "kosh_execute_cmd for 'cmd' cmd_called not NULL" );
    fail_unless( s, called_argv == NULL, "kosh_execute_cmd for 'cmd' called_argv not NULL" );
    fail_unless( s, called_argc == -1, "kosh_execute_cmd for 'cmd' called_argc not -1" );

    cmd_called = NULL; called_argv = NULL; called_argc = -1;

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"cmd for me", error ) == KOSH_NO_SUCH_COMMAND, "kosh_execute_cmd for 'cmd for me' does not return KOSH_NO_SUCH_COMMAND" );
    fail_unless( s, cmd_called == NULL, "kosh_execute_cmd for 'cmd for me' cmd_called not NULL" );
    fail_unless( s, called_argv == NULL, "kosh_execute_cmd for 'cmd for me' called_argv not NULL" );
    fail_unless( s, called_argc == -1, "kosh_execute_cmd for 'cmd for me' called_argc not -1" );

    cmd_called = NULL; called_argv = NULL; called_argc = -1;

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"cmd1", error ) == KOSH_OK, "kosh_execute_cmd for 'cmd1' does not return KOSH_OK" );
    fail_unless( s, __strcmp( cmd_called, "cmd1" ) == 0, "kosh_execute_cmd for 'cmd1' cmd_called not 'cmd1'" );
    fail_unless( s, called_argv != NULL && __wcscmp( (int*)called_argv[0], L"cmd1" ) == 0, "kosh_execute_cmd for 'cmd1' called_argv is NULL or does not match L'cmd1'" );
    fail_unless( s, called_argc == 1, "kosh_execute_cmd for 'cmd1' called_argc not 1" );

    cmd_called = NULL; called_argv = NULL; called_argc = -1;

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"cmd13", error ) == KOSH_NO_SUCH_COMMAND, "kosh_execute_cmd for 'cmd13' does not return KOSH_NO_SUCH_COMMAND" );
    fail_unless( s, cmd_called == NULL, "kosh_execute_cmd for 'cmd13' cmd_called not NULL" );
    fail_unless( s, called_argv == NULL, "kosh_execute_cmd for 'cmd13' called_argv not NULL" );
    fail_unless( s, called_argc == -1, "kosh_execute_cmd for 'cmd13' called_argc not -1" );

    cmd_called = NULL; called_argv = NULL; called_argc = -1;

    fail_unless( s, kosh_execute_cmd( &l1, (const char32_t*)L"cmd12 for the win!", error ) == KOSH_OK, "kosh_execute_cmd for 'cmd12 for the win!' does not return KOSH_OK" );
    fail_unless( s, __strcmp( cmd_called, "cmd12" ) == 0, "kosh_execute_cmd for 'cmd12' cmd_called not 'cmd12'" );
    fail_unless( s, called_argc == 4, "kosh_execute_cmd for 'cmd12 for the win!' called_argc not 4" );
    fail_unless( s, called_argv != NULL && __wcscmp( (int*)called_argv[0], L"cmd12" ) == 0, "kosh_execute_cmd argv[0] for 'cmd12 for the win!' called_argv is NULL or does not match L'cmd1'" );
    fail_unless( s, called_argv != NULL && __wcscmp( (int*)called_argv[1], L"for" ) == 0, "kosh_execute_cmd argv[1] for 'cmd12 for the win!' called_argv is NULL or does not match L'for'" );
    fail_unless( s, called_argv != NULL && __wcscmp( (int*)called_argv[2], L"the" ) == 0, "kosh_execute_cmd argv[2] for 'cmd12 for the win!' called_argv is NULL or does not match L'the'" );
    fail_unless( s, called_argv != NULL && __wcscmp( (int*)called_argv[3], L"win!" ) == 0, "kosh_execute_cmd argv[3]  for 'cmd12 for the win!' called_argv is NULL or does not match L'win!'" );

    return conclude_suite( s );
}

