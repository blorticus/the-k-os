//#include <stdio.h>
//#include <string.h>
//#include <kterm.h>
//#include <stdlib.h>
#include <sys/types.h>

/* for input from prompt */
#define INPUT_BUFFER_SIZE   100

/* for tokens on prompt line */
#define TOKEN_BUFFER_SIZE   25

#define NULL    '\0'

char input_buffer[INPUT_BUFFER_SIZE];
char token_buffer[TOKEN_BUFFER_SIZE];


enum prompt_base_command {
    EXIT,           // currently, this means halt the system
    PEEK,           // memory location or register name
    EHCO,           // echo a string
    POKE            // memory location or register name
};

//typedef struct {
//    int                     command;
//    char*                   register;
//    _U32                    memory_location;
//} prompt_value;


/* parses 'str' looking for the next sequence of characters that end with NULL or whitespace, up to 'limit' number
 * of characters.  If 'limit' is reached, chop the string to 'limit' and add NULL (so return might be 'limit' + 1).
 * Otherwise, return the string, with NULL terminator.  If zero or more whitespace followed by NULL is reached, return
 * NULL.  If 'str' is NULL, then the next_word in the most recent non-NULL 'str' entry is searched again.  (this works
 * like strtok()).  Result is written to 'buffer', and a pointer to 'buffer' is the return value. */
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

    return buffer;
}

void prompt( void ) {
//    _puts( "KoSH> " );
//    _fgets( input_buffer, INPUT_BUFFER_SIZE );
//
//    next_word( input_buffer, token_buffer, TOKEN_BUFFER_SIZE - 1 );
//
//    if (_strcmp( token_buffer, "echo 
}

int kosh_main( void ) {
//    _cls();
    return 0;    
}


//int main( void ) {
//    return 0;
//}
