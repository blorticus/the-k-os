#include <kosh/koshlib.h>
#include <kterm.h>


kosh_instruction* prompt( void ) {
    kosh_instruction* instruction = NULL;

    while (instruction == NULL) {
        _puts( "KoSH> " );
        _fgets( input_buffer, INPUT_BUFFER_SIZE );
        instruction = input_to_instruction( input_buffer );
    }

    return instruction;
}


int main( void ) {
    kosh_instruction* next_instruction;

    _cls();

    do {
        next_instruction = prompt();
    } while (next_instruction->command != EXIT);

    return 0;    
}

