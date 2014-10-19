#include <apps/kosh/kosh_cmd.h>
#include <string.h>


// if TESTING is defined, kmalloc() must be defined by the caller
#ifndef TESTING
    #include <memory/kmalloc.h>
#else
    extern void* kmalloc( size_t bytes );
#endif


kosh_error kosh_init_cmd_structure( kosh_cmd_list* l ) {
    l->head = 0;
    l->tail = 0;

    return KOSH_OK;
}



kosh_error kosh_register_cmd( kosh_cmd_list* l, const char32_t* command, kosh_cmd_ref cmd_ref ) {
    kosh_cmd_node* node;

    node = kmalloc( sizeof( kosh_cmd_node ) );
    node->cmd = cmd_ref;
    node->command_string = (char32_t*)command;
    node->next = NULL;

    if (!l->head) {  // ASSERT: list is empty
        l->head = node;
        l->tail = node;
    }
    else {
        l->tail->next = node;
        l->tail = node;
    }

    return KOSH_OK;
}


kosh_error kosh_execute_cmd( kosh_cmd_list* l, const char32_t* command_line ) {
    unsigned int token_num = 0;
    unsigned int token_len = 0;
    char32_t* t;
    kosh_cmd_node* node;

    if (command_line == NULL || command_line[0] == NULL)
        return KOSH_EMPTY_COMMAND;

    // tokenize the command line
    t = (char32_t*)command_line;

    // remove any leading whitespace
    if (*t == (char32_t)' ') {
        while ((*t++ = (char32_t)' '))
            ;
    }

    while (*t) {
        if (*t == (char32_t)' ') {
            while (*t == (char32_t)' ') {
                t++;
            }

            // null terminate previous token
            l->parsed_tokens[token_num][token_len] = NULL;
            l->token_p[token_num] = (char32_t*)l->parsed_tokens[token_num];

            if (++token_num >= KOSH_CMD_MAX_TOKENS && *t)
                // the next char isn't NULL and we've used all token slots...
                return KOSH_COMMAND_TOO_LONG;

            token_len = 0;
        }
        else { // ASSERT: not a space
            if (token_len >= KOSH_CMD_MAX_TOKEN_LENGTH - 1)
                return KOSH_COMMAND_TOO_LONG;

            l->parsed_tokens[token_num][token_len++] = *t++;
        }
    }

    l->parsed_tokens[token_num][token_len] = 0;
    l->token_p[token_num] = (char32_t*)l->parsed_tokens[token_num];

    // find the matching command
    for (node = l->head; node; node = node->next)
        if (wcsncmp( node->command_string, (const char32_t*)(l->parsed_tokens[0]), KOSH_CMD_MAX_TOKEN_LENGTH ) == 0)
            return node->cmd( (char32_t**)(l->token_p), token_num + 1 );

    return KOSH_NO_SUCH_COMMAND;
}
