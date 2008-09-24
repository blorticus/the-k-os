#include <kosh/koshlib.h>
#include <kterm.h>

#define INPUT_BUFFER_SIZE   100

char input_buffer[INPUT_BUFFER_SIZE];

kosh_instruction* prompt( void ) {
    kosh_instruction* instruction;

    do {
        kterm_puts( "KoSH> " );
        kterm_fgets( input_buffer, INPUT_BUFFER_SIZE );
        instruction = input_to_instruction( input_buffer );
    } while (instruction->command == _ERROR_ || instruction->command == _EMPTY_);

    return instruction;
}


int main( void ) {
    kosh_instruction* next_instruction;

    kterm_create();

    do {
        next_instruction = prompt();

        /* should not get _ERROR_ or _EMPTY_ from the call to prompt() */
        switch (next_instruction->command) {
            case ECHO:
                kterm_puts( next_instruction->remaining_command_line );  /* will include inputed newline */
                break;

            case PEEK:
                kterm_puts( "peek\n" );
                break;

            case POKE:
                kterm_puts( "poke\n" );
                break;

            default:
                break;
        }
    } while (next_instruction->command != EXIT);

    return 0;    
}

