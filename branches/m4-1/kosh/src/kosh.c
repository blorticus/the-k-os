#include <kosh/koshlib.h>

int main( void ) {
    kosh_instruction* next_instruction;

    _cls();

    do {
        next_instruction = prompt();
    } while (next_instruction->command != EXIT);

    return 0;    
}

