#include <lcheck.h>
#include <koshlib.h>
#include <string.h>


int main( void ) {
    lcheck_suite* s;

    s = create_suite( "kosh_input_to_instruction" );

    kosh_instruction* i;

    i = input_to_instruction( NULL );

    fail_unless( s, i->command == _EMPTY_,
                 "input_to_instruction() doesn't set command to _EMPTY_ when input is NULL" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is NULL" );

    i = input_to_instruction( "" );

    fail_unless( s, i->command == _EMPTY_,
                 "input_to_instruction() doesn't set command to _EMPTY_ when input is empty string" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is empty string" );

    i = input_to_instruction( "exit" );

    fail_unless( s, i->command == EXIT,
                 "input_to_instruction() doesn't set command to EXIT when input is word 'exit'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit'" );

    i = input_to_instruction( "peek" );

    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is 'peek' w/o param" );
    fail_unless( s, __strlen( i->error ) > 0,
                 "input_to_instruction() doesn't set error when input is 'peek' w/o param" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'peek' w/o param" );

    i = input_to_instruction( "exit foo" );

    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is 'exit foo'" );
    fail_unless( s, __strlen( i->error ) > 0,
                 "input_to_instruction() doesn't set error when input is 'exit foo'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit'" );

    i = input_to_instruction( "peek eax" );

    fail_unless( s, i->command == PEEK,
                 "input_to_instruction() doesn't set command to PEEK when input is 'peek eax'" );
    fail_unless( s, !(i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET),
                 "input_to_instruction() sets MEMADDR flag when input is 'peek eax'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET),
                 "input_to_instruction() fails to set REG flag when input is 'peek eax'" );
    fail_unless( s, i->subcommand == EAX,
                 "input_to_instruction() fails to set subcommand to EAX when input is 'peek eax'" );

    i = input_to_instruction( "exit " );

    fail_unless( s, i->command == EXIT,
                 "input_to_instruction() doesn't set command to EXIT when input is word 'exit '" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit '" );

    i = input_to_instruction( "  exit" );

    fail_unless( s, i->command == EXIT,
                 "input_to_instruction() doesn't set command to EXIT when input is word 'exit '" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit '" );

    i = input_to_instruction( " peek EFLAGS  " );

    fail_unless( s, i->command == PEEK,
                 "input_to_instruction() doesn't set command to PEEK when input is ' peek EFLAGS  '" );
    fail_unless( s, !(i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET),
                 "input_to_instruction() sets MEMADDR flag when input is ' peek EFLAGS  '" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET),
                 "input_to_instruction() fails to set REG flag when input is ' peek EFLAGS  '" );
    fail_unless( s, i->subcommand == EFLAGS,
                 "input_to_instruction() fails to set subcommand to EFLAGS when input is ' peek EFLAGS  '" );

    i = input_to_instruction( "exit2" );

    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is 'exit2'" );
    fail_unless( s, __strlen( i->error ) > 0,
                 "input_to_instruction() doesn't set error when input is 'exit2'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit2'" );

    i = input_to_instruction( "peek 0xa1f35d" );

    fail_unless( s, i->command == PEEK,
                 "input_to_instruction() doesn't set command to PEEK when input is 'peek 0xa1f35d'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET),
                 "input_to_instruction() fails to set MEMADDR flag when input is 'peek 0xa1f35d'" );
    fail_unless( s, !(i->flags & KOSH_INSTRUCTION_FLAG_REG_SET),
                 "input_to_instruction() fails to set REG flag when input is 'peek 0xa1f35d'" );
    fail_unless( s, i->memory_location == 0xa1f35d,
                 "input_to_instruction() fails to set memory_location to 0xa1f35d when input is 'peek 0xa1f35d'" );

    i = input_to_instruction( "echo" );

    fail_unless( s, i->command == ECHO,
                 "input_to_instruction() doesn't set command to ECHO when input is 'echo'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is word 'exit'" );
    fail_unless( s, __strlen( i->remaining_command_line ) == 0,
                 "input_to_instruction() remaining_command_line is not empty on input 'echo'" );

    i = input_to_instruction( " POKE   Ss" );

    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is ' POKE Ss'" );
    fail_unless( s, __strlen( i->error ) > 0,
                 "input_to_instruction() fails to set error when input is ' POKE Ss'" );

    i = input_to_instruction( " echo  foo bar 'baz\"" );

    fail_unless( s, i->command == ECHO,
                 "input_to_instruction() doesn't set command to ECHO when input is ' echo  foo bar 'baz\"'" );
    fail_unless( s, (i->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) == 0 && (i->flags & KOSH_INSTRUCTION_FLAG_REG_SET) == 0,
                 "input_to_instruction() sets MEMADDR and/or REG flags when input is  ' echo   foo bar 'baz\"'" );
    fail_unless( s, __strcmp( i->remaining_command_line, "foo bar 'baz\"" ) == 0,
                 "input_to_instruction() remaining_command_line is not empty on input ' echo  foo bar 'baz\"'" );

    i = input_to_instruction( "raise" );
    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is 'raise'" );

    i = input_to_instruction( "raise " );
    fail_unless( s, i->command == _ERROR_,
                 "input_to_instruction() doesn't set command to _ERROR_ when input is 'raise '" );

    i = input_to_instruction( "raise foo" );
    fail_unless( s, i->command == RAISE,
                 "input_to_instruction() doesn't set command to RAISE when input is 'raise foo'" );
    fail_unless( s, i->subcommand == INT_INPUT,
                 "input_to_instruction() doesn't set subcommand to INT_INPUT when input is 'raise foo'" );
    fail_unless( s, __strcmp( i->remaining_command_line, "foo" ) == 0,
                 "input_to_instruction() doesn't set remaining_command_line to 'foo' when input is 'raise foo'" );

    i = input_to_instruction( "raise div_0" );
    fail_unless( s, i->command == RAISE,
                 "input_to_instruction() doesn't set command to RAISE when input is 'raise div_0'" );
    fail_unless( s, i->subcommand == DIVIDE_BY_ZERO,
                 "input_to_instruction() doesn't set subcommand to DIVIDE_BY_ZERO when input is 'raise div_0'" );

    i = input_to_instruction( "raise 5" );
    fail_unless( s, i->command == RAISE,
                 "input_to_instruction() doesn't set command to RAISE when input is 'raise 5'" );
    fail_unless( s, i->subcommand == INT_INPUT,
                 "input_to_instruction() doesn't set subcommand to INT_INPUT when input is 'raise 5'" );
    fail_unless( s, __strcmp( i->remaining_command_line, "5" ) == 0,
                 "input_to_instruction() doesn't set remaining_command_line to 'foo' when input is 'raise 5'" );


    return conclude_suite( s );
}
