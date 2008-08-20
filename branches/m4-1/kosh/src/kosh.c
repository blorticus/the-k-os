#include <koshlib.h>
#include <kterm.h>
#include <sys/types.h>
#include <string.h>

/* for input from prompt */
#define INPUT_BUFFER_SIZE   100

/* for tokens on prompt line */
#define TOKEN_BUFFER_SIZE   25


#define NULL    '\0'

char input_buffer[INPUT_BUFFER_SIZE];
char token_buffer[TOKEN_BUFFER_SIZE];

/*
typedef enum {
    _ERROR_,        // an error occurred
    _EMPTY_,        // no command supplied
    EXIT,           // currently, this means halt the system
    PEEK,           // memory location or register name
    EHCO,           // echo a string
    POKE            // memory location or register name
} kosh_base_command;

typedef enum {
    AL, AH, BL, BH, CL, CH, DL, DH,
    AX, BX, CX, DX,
    EAX, EBX, ECX, EDX,
    SS, CS, DS, ES, FS, GS,
    SI, DI,
    ESI, EDI,
    BP, EBP,
    SP, ESP,
    EFLAGS
} register_name;


typedef struct {
    kosh_base_command       command;
    register_name           reg;
    memaddr                 memory_location;
    char*                   remaining_command_line;
    const char*             error;
} kosh_instruction;
*/


/* parses 'str' looking for the next sequence of characters that end with NULL or whitespace, up to 'limit' number
 * of characters.  If 'limit' is reached, chop the string to 'limit' and add NULL (so return might be 'limit' + 1).
 * Otherwise, return the string, with NULL terminator.  If zero or more whitespace followed by NULL is reached, return
 * NULL string.  If 'str' is NULL, then the next_word in the most recent non-NULL 'str' entry is searched again.  (this works
 * like strtok()).  Result is written to 'buffer', and a pointer to first character after 'buffer' word in 'str' is returned. */
char* last_searched = NULL;

char* next_word( char* str, char* buffer, _U32 limit ) {
    int i;

    if (str == NULL) {
        if (last_searched == NULL)
            return NULL;
    }
    else {
        last_searched = str;
    }

    /* strip off any leading whitespace */
    while (*last_searched == ' ' || *last_searched == '\t' || *last_searched == '\n')
        last_searched++;

    if (*last_searched == NULL) {
        buffer[0] = NULL;
        return buffer;
    }

    for (i = 0; i < limit && *last_searched != ' ' && *last_searched != '\t' && *last_searched != '\n' && *last_searched != NULL; i++)
        *buffer++ = *last_searched++;

    *buffer = NULL;

    return last_searched;
}

/* look to see if regname matches a register name in any mix of case.  If so, return the 'register_name' value; otherwise, if it does
 * not match a register name, return -1. */
int match_register( const char* regname ) {
    // this would probably better be done with a hash table or something similar
    if (regname[0] == 'A' || regname[0] == 'a') {
        if ((regname[1] == 'L' || regname[1] == 'l') && regname[2] == NULL)
            return AL;
        else if ((regname[1] == 'H' || regname[1] == 'h') && regname[2] == NULL)
            return AH;
        else if ((regname[1] == 'X' || regname[1] == 'x') && regname[2] == NULL)
            return AX;
    }
    else if (regname[0] == 'B' || regname[0] == 'b') {
        if ((regname[1] == 'L' || regname[1] == 'l') && regname[2] == NULL)
            return BL;
        else if ((regname[1] == 'H' || regname[1] == 'h') && regname[2] == NULL)
            return BH;
        else if ((regname[1] == 'P' || regname[1] == 'p') && regname[2] == NULL)
            return BP;
        else if ((regname[1] == 'X' || regname[1] == 'x') && regname[2] == NULL)
            return BX;
    }
    else if (regname[0] == 'C' || regname[0] == 'c') {
        if ((regname[1] == 'L' || regname[1] == 'l') && regname[2] == NULL)
            return CL;
        else if ((regname[1] == 'H' || regname[1] == 'h') && regname[2] == NULL)
            return CH;
        else if ((regname[1] == 'S' || regname[1] == 's') && regname[2] == NULL)
            return CS;
        else if ((regname[1] == 'X' || regname[1] == 'x') && regname[2] == NULL)
            return CX;
    }
    else if (regname[0] == 'D' || regname[0] == 'd') {
        if ((regname[1] == 'L' || regname[1] == 'l') && regname[2] == NULL)
            return DL;
        else if ((regname[1] == 'H' || regname[1] == 'h') && regname[2] == NULL)
            return DH;
        else if ((regname[1] == 'I' || regname[1] == 'i') && regname[2] == NULL)
            return DI;
        else if ((regname[1] == 'S' || regname[1] == 's') && regname[2] == NULL)
            return DS;
        else if ((regname[1] == 'X' || regname[1] == 'x') && regname[2] == NULL)
            return DX;
    }
    else if (regname[0] == 'E' || regname[0] == 'e') {
        if ((regname[1] == 'A' || regname[1] == 'a') && (regname[2] == 'X' || regname[2] == 'x') && regname[3] == NULL)
            return EAX;
        else if (regname[1] == 'B' || regname[1] == 'b') {
            if ((regname[2] == 'X' || regname[2] == 'x') && regname[3] == NULL)
                return EBX;
            else if ((regname[2] == 'P' || regname[2] == 'p') && regname[3] == NULL)
                return EBP;
        }
        else if ((regname[1] == 'C' || regname[1] == 'c') && (regname[2] == 'X' || regname[2] == 'x') && regname[3] == NULL)
            return ECX;
        else if (regname[1] == 'D' || regname[1] == 'd') {
            if ((regname[2] == 'X' || regname[2] == 'x') && regname[3] == NULL)
                return EDX;
            else if ((regname[2] == 'I' || regname[2] == 'i') && regname[3] == NULL)
                return EDI;
        }
        else if (regname[1] == 'S' || regname[1] == 's') {
            if (regname[2] == NULL)
                return ES;
            else if ((regname[2] == 'I' || regname[2] == 'i') && regname[3] == NULL)
                return ESI;
            else if ((regname[2] == 'P' || regname[2] == 'p') && regname[3] == NULL)
                return ESP;
        }
        else {
            // XXX: check for EFLAGS
        }
    }
    else if (regname[0] == 'F' || regname[0] == 'f') {
        if ((regname[1] == 'S' || regname[1] == 's') && regname[2] == NULL)
            return FS;
    }
    else if (regname[0] == 'G' || regname[0] == 'g') {
        if ((regname[1] == 'S' || regname[1] == 's') && regname[2] == NULL)
            return GS;
    }
    else if (regname[0] == 'S' || regname[0] == 's') {
        if ((regname[1] == 'I' || regname[1] == 'i') && regname[2] == NULL)
            return SI;
        else if ((regname[1] == 'P' || regname[1] == 'p') && regname[2] == NULL)
            return SP;
        else if ((regname[1] == 'S' || regname[1] == 's') && regname[2] == NULL)
            return SS;
    }

    return -1;  /* matched nothing */
}


/* Parses a command "word" (no whitespace permitted) and determines whether it is memory location or a register name (or neither).
 * Memory locations must begin with 0x and must be hex digits only.  Registers must be Intel canonical format names for 8-, 16- or
 * 32-bit registers in any case (even mixed case).  'word' must be NULL terminated.  If 'word' is not null-terminated or contains
 * whitespace, results are not defined.  If 'word' is a memory location, its value is converted to 'memaddr' type and fed to
 * instruction.memory_location (and instruction.reg is set to -1).  If it appears to be a register name, instruction.reg is set to
 * the enum 'register_name' value and instruction.memory_location is set to 0.  If 'word' appears to be invalid, instruction.error
 * is set and zero is returned.  Otherwise, non-zero is returned.  If 'word' is a memory location, and it specifies a value outside
 * the bounds of the type 'memaddr', instruction.error will be set and zero will be returned.
 */
int extract_reg_or_mem( char* word, kosh_instruction* instruction ) {
    if (word == NULL || word[0] == NULL)
        return 0;

    // memory locations must be hex, and must start with 0x
    memaddr memloc      = 0;
    memaddr last_memloc = 0;  // used to compare to see if wrap occurs
  
    if (word[0] == '0' && word[1] == 'x') {
        word += 2;
        while (*word) {
            last_memloc = memloc;
            if (*word >= 'a' && *word <= 'f') {
                memloc = (memloc << 4) + (*word - 'a' + 10);
            }
            else if (*word >= 'A' && *word <= 'F') {
                memloc = (memloc << 4) + (*word - 'A' + 10);
            }
            else if (*word >= '0' && *word <= '9') {
                memloc = (memloc << 4) + (*word - '0');
            }
            else {
                instruction->error = "Malformed Memory Location";
                return 0;
            }
            if (memloc < last_memloc) {
                instruction->error = "Memory Location Out-of-Bounds";
                return 0;
            }
        }
        return 1;
    }
    else {    // is a register or is invalid
    }

    return 1;
}


kosh_instruction* input_to_instruction( char* input ) {
    return NULL;
//    kosh_instruction instruction;
//    char* after_command;
//
//    after_command = next_word( input, token_buffer, TOKEN_BUFFER_SIZE - 1 );
//
//    if (token_buffer[0] == NULL) {
//        instruction.command = _EMPTY_;
//        instruction.error   = NULL;
//    }
//    else if (strcmp( token_buffer, "exit" ) == 0) {
//        next_word( NULL, token_buffer, TOKEN_BUFFER_SIZE - 1 );
//        if (token_buffer[0] != NULL) {
//            instruction.command = _ERROR_;
//            instruction.error   = "Extra Input After Command";
//        }
//        else {
//            instruction.command = EXIT;
//        }
//    }
//    else if (strcmp( token_buffer, "peek" ) == 0) {
//        next_word( NULL, token_buffer, TOKEN_BUFFER_SIZE - 1 );
//        if (token_buffer[0] == NULL) {
//            instruction.command = _ERROR_;
//            instruction.error   = "peek <mem|reg>";
//        }
//        else {
//            if (extract_reg_or_mem( token_buffer, &kosh_instruction ) == 0) {
//                kosh_instruction.command = _ERROR_;
//                kosh_instruction.error   = "peek <mem|reg>";
//            }
//            // otherwise, extract_reg_or_mem() has changed kosh_instruction
//        }
//    }
//    else if (strcmp( token_buffer, "poke" ) == 0) {
//        next_word( NULL, token_buffer, TOKEN_BUFFER_SIZE - 1 );
//        if (token_buffer[0] == NULL) {
//            instruction.command = _ERROR_;
//            instruction.error   = "peek <mem|reg>";
//        }
//        else {
//            if (extract_reg_or_mem( token_buffer, &kosh_instruction ) == 0) {
//                instruction.command = _ERROR_;
//                instruction.error   = "peek <mem|reg>";
//            }
//            // otherwise, extract_reg_or_mem() has changed kosh_instruction
//    }
//    else if (strcmp( token_buffer, "echo" ) == 0) {
//        while (*after_command = ' ' || *after_command == '\t' || *after_command == '\n')
//            after_command++;
//
//        instruction.command                = ECHO;
//        instruction.remaining_command_line = after_command;
//    }
//    else {
//        instruction.command = _ERROR_;
//        instruction.error   = "Invalid Command";
//    }
//
//    return &kosh_instruction;
}


kosh_instruction* prompt( void ) {
    kosh_instruction* instruction = NULL;

    while (instruction == NULL) {
        _puts( "KoSH> " );
        _fgets( input_buffer, INPUT_BUFFER_SIZE );
        instruction = input_to_instruction( input_buffer );
    }

    return instruction;
}


int kosh_main( void ) {
    kosh_instruction* next_instruction;

    _cls();

    do {
        next_instruction = prompt();
    } while (next_instruction->command != EXIT);

    return 0;    
}


//int main( void ) {
//    return 0;
//}
