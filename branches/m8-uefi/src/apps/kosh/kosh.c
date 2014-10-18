#include <sys/types.h>
#include <apps/kosh.h>


/*

    - help                  : help information
    - info registers        : dump register values
    - info memmap           : dump memory map
    - peek <addr>           : retrieve memory value
    - poke <addr> <val>     : set value at <addr> to <val>
    - pci bus scan          : scan the PCI bus and print values

*/

/* number of commands */
#define CMD_COUNT   5

typedef enum {
    KOSH_OK             = 0,
    WRONG_ARG_CNT       = 1,
    WRONG_ARG_VALUE     = 2
} kosh_error;


/**
 * For a matching command, execute a method of this type.  Pass the method
 *  a list of character arguments and count of the arguments in the list.
 **/
typedef kosh_error (*kosh_cmd_ref)( char32_t**, int );


 /**
  * Each node contains a match of the command string, and an executable ref
  **/
typedef struct kosh_cmd_node {
    char32_t*               command_string;
    kosh_cmd_ref            cmd;
    kosh_cmd_node*          list_next;
    kosh_cmd_node*          list_prev;
} kosh_cmd_node;


typedef struct {
    kosh_cmd_node*          list_head;  // first list node (null if empty)
    kosh_cmd_node*          list_tail;  // last list node (null if empty)
} kosh_system;


//kosh_error cmd_help( char32_t** argv, int argc, char32_t* error ) {
//
//}
//
//
//kosh_error cmd_info( char32_t** argv, int argc, char32_t* error ) {
//
//}
//
//
//kosh_error cmd_peek( char32_t** argv, int argc, char32_t* error ) {
//
//}
//
//
//kosh_error cmd_poke( char32_t** argv, int argc, char32_t* error ) {
//
//}
//
//
//kosh_error cmd_pci( char32_t** argv, int argc, char32_t* error ) {
//
//}


kosh_cmd_node nodes[CMD_COUNT];

kosh_error init_kosh_system( kosh_system* ks ) {
    nodes[0].command_string = "help";
    nodes[0].cmd = cmd_help;
    nodes[0].list_next = &nodes[1];
    nodes[0].list_prev = NULL;

    nodes[1].command_string = "info";
    nodes[1].cmd = cmd_info;
    nodes[1].list_next = &nodes[2];
    nodes[1].list_prev = &nodes[0];

    nodes[2].command_string = "peek";
    nodes[2].cmd = cmd_peek;
    nodes[2].list_next = &nodes[3];
    nodes[2].list_prev = &nodes[1];

    nodes[3].command_string = "poke";
    nodes[3].cmd = cmd_poke;
    nodes[3].list_next = &nodes[4];
    nodes[3].list_prev = &nodes[2];

    nodes[4].command_string = "pci";
    nodes[4].cmd = cmd_pci;
    nodes[4].list_next = NULL;
    nodes[4].list_prev = &nodes[3];

    ks->list_head = &nodes[0];
    ks->list_tail = &nodes[4];

    return KOSH_OK;
}


/**
 * For a give command (command_token) associate it with the command reference (cmd_ref) which will be
 *  passed the arguments (kcl)
 **/
//void register_command( kosh_system* ks, const char32_t* command_token, kosh_cmd_ref cmd_ref, kosh_cmd_list** kcl ) {
//}


void kosh_start_shell( kosh_shell* shell, term_entry* term ) {
//    kosh_cmd_list[16][8] kcl;

    shell->term = term;
    term_printf( term, "Welcome to KOSH!" );
}
