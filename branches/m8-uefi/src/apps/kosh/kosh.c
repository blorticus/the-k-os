#include <sys/types.h>
#include <apps/kosh/kosh.h>
#include <apps/kosh/kosh_cmd.h>
#include <string.h>


/*

    - help                  : help information
    - info registers        : dump register values
    - info memmap           : dump memory map
    - peek <addr>           : retrieve memory value
    - poke <addr> <val>     : set value at <addr> to <val>
    - pci bus scan          : scan the PCI bus and print values

*/

kosh_error cmd_help( char32_t** argv __attribute__((unused)), int argc __attribute__((unused)), char32_t* error __attribute__((unused)) ) {
    error = (char32_t*)0;
    return KOSH_OK;
}


kosh_error cmd_info( char32_t** argv __attribute__((unused)), int argc __attribute__((unused)), char32_t* error __attribute__((unused)) ) {
    error = (char32_t*)0;
    return KOSH_OK;
}


kosh_error cmd_peek( char32_t** argv __attribute__((unused)), int argc __attribute__((unused)), char32_t* error __attribute__((unused)) ) {
    error = (char32_t*)0;
    return KOSH_OK;
}


kosh_error cmd_poke( char32_t** argv __attribute__((unused)), int argc __attribute__((unused)), char32_t* error __attribute__((unused)) ) {
    error = (char32_t*)0;
    return KOSH_OK;
}


kosh_error cmd_pci( char32_t** argv __attribute__((unused)), int argc __attribute__((unused)), char32_t* error __attribute__((unused)) ) {
    error = (char32_t*)0;
    return KOSH_OK;
}


void kosh_start_shell( kosh_shell* ks, term_entry* term ) {
    ks->term = term;

    if (kosh_register_cmd( &(ks->cmd_list), (const char32_t*)L"help", cmd_help ) != KOSH_OK) return;
    if (kosh_register_cmd( &(ks->cmd_list), (const char32_t*)L"info", cmd_info ) != KOSH_OK) return;
    if (kosh_register_cmd( &(ks->cmd_list), (const char32_t*)L"peek", cmd_peek ) != KOSH_OK) return;
    if (kosh_register_cmd( &(ks->cmd_list), (const char32_t*)L"poke", cmd_poke ) != KOSH_OK) return;
    if (kosh_register_cmd( &(ks->cmd_list), (const char32_t*)L"pci",  cmd_pci  ) != KOSH_OK) return;

    term_cls( term );
    term_printf( term, "KoSH> " );
}
