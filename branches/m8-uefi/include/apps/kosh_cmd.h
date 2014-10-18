#ifndef __KOSH_CMD__
#define __KOSH_CMD__

#include <sys/types.h>

#define KOSH_CMD_MAX_TOKEN_LENGTH   16
#define KOSH_CMD_MAX_TOKENS         8

/**************
 * DESCRIPTION:     The KoSH accepts commands on the terminal.  The commands are of the
 *                  form <cmd> [<subcmds...>] [<params...>].  Tokens are separated by
 *                  one or more space characers (ASCII 20h).  Currently, quotes are not
 *                  special.  When a <cmd> token is found, it is matched against a command
 *                  search tree.  When the matching command is found, a function of the
 *                  type 'kosh_cmd_ref' (see below) is invoked.  'argv' always starts with
 *                  the <cmd> token.  The remaining items are the remaining tokens.  'argc'
 *                  is the token count (including the <cmd> token, so it is always at least
 *                  1).
 * IMPLEMENTATION:  Currently, a singly linked list contains nodes with a string (matching
 *                  the <cmd> token, case-sensitive) and a 'kosh_cmd_ref', which is executed
 *                  on match.  This is obviously inefficiently O(n) for scanning and could
 *                  be improved by creating a binary search tree based on the <cmd> token.  But
 *                  for a small command size, the performance difference is likely to be quite
 *                  small, and the linked list is easier to implement than the tree.
 * RE-ENTRANT:      YES
 ***************/

typedef struct kosh_cmd_node {
    char32_t*               command_string;
    kosh_cmd_ref            cmd;
    kosh_cmd_node*          next;
} kosh_cmd_node;


typedef struct kosh_cmd_list {
    kosh_cmd_node*          head;
    kosh_cmd_node*          tail;   // tail facilitates insertion

    char32_t[KOSH_CMD_MAX_TOKENS][KOSH_CMD_MAX_TOKEN_LENGTH] parsed_tokens;  // used when parsing out commands
} kosh_cmd_list;


typedef enum {
    KOSH_OK                 = 0,
    KOSH_WRONG_ARG_CNT      = 1,
    KOSH_WRONG_ARG_VALUE    = 2,
    KOSH_NO_SUCH_COMMAND    = 3,
    KOSH_EMPTY_COMMAND      = 4,
    KOSH_COMMAND_TOO_LONG   = 5
} kosh_error;

typedef kosh_error (*kosh_cmd_ref)( char32_t**, int );


/***
 * Initialize the kosh command list data structure.  Must be done before using the list structure.
 ***/
kosh_error kosh_init_cmd_structure( kosh_cmd_list* l );


/***
 * Register a command, providing the command token string and the command pointer ref.  If
 * 'command' contains a space, it will never match any token, but will succeed in registration.
 ***/
kosh_error kosh_register_cmd( kosh_cmd_list* l, const char32_t* command, kosh_cmd_ref* cmd_ref );


/***
 * Given a command_line (a command token followed by zero or more space-delimited tokens), try to find
 * the matching command in 'l'.  If found, invoke the corresponding kost_cmd_ref and return its result.
 * If the command token does not match any kosh_cmd_list items, return KOSH_NO_SUCH_COMMAND.  If command_line
 * is NULL or empty, return KOSH_EMPTY_COMMAND.  If there are more than KOSH_CMD_MAX_TOKENS tokens in the
 * command_line, or if any token exceeds the KOSH_CMD_MAX_TOKEN_LENGTH, then KOSH_COMMAND_TOO_LONG is returned.
 ***/
kosh_error kosh_execute_cmd( kosh_cmd_list* l, const char32_t* command_line );



#endif
