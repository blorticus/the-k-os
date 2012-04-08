#ifndef __KOSH_KOSHLIB_H__
#define __KOSH_KOSHLIB_H__


#include <sys/types.h>

typedef enum {
    _ERROR_,        // an error occurred
    _EMPTY_,        // no command supplied
    EXIT,           // currently, this means halt the system
    PEEK,           // memory location or register name
    ECHO,           // echo a string
    POKE,           // memory location or register name
    DUMPREGS,       // dump important memory registers
    BIOS,           // dump bios area of memory
    INTDIAG,        // interrupt diagnostic
    HELP,           // print help
    CPUID,          // CPUID support and info
    KERNEL_INFO,    // information about the kernel
    KMALLOC,        // test for kmalloc
    PCI_BUS_SCAN,   // scan PCI bus
    PCI_BUS_SCAN_CLASS, // scan PCI bus for class, remainig_command_line is int for class match
    TASK_START,     // testing multi-tasking, task start
    TASK_END,       // testing multi-tasking, task end
    TASK_KILL,      // testing multi-tasking, kill by task id
    TEST,           // for temporary testing of features
    PAGING,         // enable or disable screen paging
    RAISE,          // raise an exception or interrupt
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

typedef enum {
    DIVIDE_BY_ZERO
} exception;

typedef struct {
    kosh_base_command       command;
    int                     subcommand;  // enum type register_name or exception
    memaddr                 memory_location;
    char*                   remaining_command_line;
    const char*             error;
    u8                      flags;
} kosh_instruction;


/* the flags for kosh_instruction */
#define KOSH_INSTRUCTION_FLAG_MEMADDR_SET       0x01
#define KOSH_INSTRUCTION_FLAG_REG_SET           0x02


/* parses 'str' looking for the next sequence of characters that end with NULL or whitespace, up to 'limit' number
 * of characters.  If 'limit' is reached, chop the string to 'limit' and add NULL (so return might be 'limit' + 1).
 * Otherwise, return the string, with NULL terminator.  If zero or more whitespace followed by NULL is reached, return
 * NULL string.  If 'str' is NULL, then the next_word in the most recent non-NULL 'str' entry is searched again.  (this works
 * like strtok()).  Result is written to 'buffer', and a pointer to first character after 'buffer' word in 'str' is returned. */
char* next_word( char* str, char* buffer, u32 limit );


/* look to see if regname matches a register name in any mix of case.  If so, return the 'register_name' value; otherwise, if it does
 * not match a register name, return -1. */
int match_register( const char* regname );


/* Parses a command "word" (no whitespace permitted) and determines whether it is memory location or a register name (or neither).
 * Memory locations must begin with 0x and must be hex digits only.  Registers must be Intel canonical format names for 8-, 16- or
 * 32-bit registers in any case (even mixed case).  'word' must be NULL terminated.  If 'word' is not null-terminated or contains
 * whitespace, results are not defined.  If 'word' is a memory location, its value is converted to 'memaddr' type and fed to
 * instruction.memory_location (and instruction.reg is set to -1).  If it appears to be a register name, instruction.reg is set to
 * the enum 'register_name' value and instruction.memory_location is set to 0.  If 'word' appears to be invalid, instruction.error
 * is set and zero is returned.  Otherwise, non-zero is returned.  If 'word' is a memory location, and it specifies a value outside
 * the bounds of the type 'memaddr', instruction.error will be set and zero will be returned.
 */
int extract_reg_or_mem( char* word, kosh_instruction* instruction );


/* convert input from a KoSH command shell into a 'kosh_instruction' */
kosh_instruction* input_to_instruction( char* input );


/* prompt for a KoSH command shell line */
kosh_instruction* prompt( void );



#endif
