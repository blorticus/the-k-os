#include <kosh/koshlib.h>
#include <video/kterm.h>
#include <multiboot.h>
#include <stdio.h>

#define INPUT_BUFFER_SIZE   100

char input_buffer[INPUT_BUFFER_SIZE];

kosh_instruction* prompt( void ) {
    kosh_instruction* instruction;

    for ( ;; ) {
        kterm_printf( "KoSH> " );
        kterm_readline( input_buffer, INPUT_BUFFER_SIZE );
        instruction = input_to_instruction( input_buffer );

        if (instruction->command == _ERROR_) {
            kterm_printf( "ERROR: %s\n", instruction->error );
        }
        else if (instruction->command == _EMPTY_) {
            kterm_printf( "_EMPTY_\n" );
        }
        else {
            break;
        }
    }

    return instruction;
}


/* Multiboot_relocated_info->drive_info is four bytes: [partition-sub3, partition-sub2, partition1, bios_drive].
 * This routine kterm_puts() each in a single line as:
 *  drive 0xnn [name], partition 0xnn:0xnn:0xnn
 * for part1, part2, part3 as partitions and <name> is fd0 (for 1st floppy), fd1 (for second floppy), hd0 (for first hd), hd1 (for second hd) or unk (for unknown)
 */
void puts_bios_drive_info( u32 drive_info ) {
    u8 drive = (u8)(drive_info >> 24);

    kterm_printf( "drive 0x%x [%s], partition 0x%x:0x%x:0x%x",
                        drive,
                        (drive == 0x00 ? "fd0" : (drive == 0x01 ? "fd1" : (drive == 0x80 ? "hd0" : (drive == 0x81 ? "hd1" : "unknown")))),
                        (u8)(drive_info >> 16),
                        (u8)(drive_info >> 8), 
                        (u8)(drive_info) );
}


/* multiboot_mmap_info->mmap_length is u32, and ->mmap_addr is [base_addr_low, base_addr_high, length_low, length_high, type]
 * Put the mmap info assuming it follows the literal:
 * mmap:\n
 * with one line for each entry
 */
typedef struct multiboot_memory_map {
    u32 size;
    u32 base_addr_low;
    u32 base_addr_high;
    u32 length_low;
    u32 length_high;
    u32 type;
} mb_mmap_t;


void puts_multiboot_mmap_info( u32 length, const void* struct_base_addr ) {
    mb_mmap_t *mmap;

    kterm_printf( "  Map Base Addr = 0x%x, Map Length = %i\n", (u32)struct_base_addr, length );

    for (mmap = (mb_mmap_t*)struct_base_addr;
         (u32)mmap < (u32)struct_base_addr + length;
         mmap = ((mb_mmap_t*)(u32)mmap + mmap->size + sizeof(mmap->size))) {
        kterm_printf( "  - Element:\n    size = %i, base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
                mmap->size, mmap->base_addr_high, mmap->base_addr_low, mmap->length_high, mmap->length_low, mmap->type );
    }
}


#define M_GR_EAX(var)      ({ asm volatile( "movl %%eax, %0;" : "=r"(var) ); })
#define M_GR_AX(var)       ({ asm volatile( "movw %%ax,  %0;" : "=r"(var) ); })
#define M_GR_AL(var)       ({ asm volatile( "movb %%al,  %0;" : "=r"(var) ); })
#define M_GR_AH(var)       ({ asm volatile( "movb %%ah,  %0;" : "=r"(var) ); })
#define M_GR_EBX(var)      ({ asm volatile( "movl %%ebx, %0;" : "=r"(var) ); })
#define M_GR_BX(var)       ({ asm volatile( "movw %%bx,  %0;" : "=r"(var) ); })
#define M_GR_BL(var)       ({ asm volatile( "movb %%bl,  %0;" : "=r"(var) ); })
#define M_GR_BH(var)       ({ asm volatile( "movb %%bh,  %0;" : "=r"(var) ); })
#define M_GR_ECX(var)      ({ asm volatile( "movl %%ecx, %0;" : "=r"(var) ); })
#define M_GR_CX(var)       ({ asm volatile( "movw %%cx,  %0;" : "=r"(var) ); })
#define M_GR_EDX(var)      ({ asm volatile( "movl %%edx, %0;" : "=r"(var) ); })
#define M_GR_DX(var)       ({ asm volatile( "movw %%dx,  %0;" : "=r"(var) ); })

#define M_GR_SS(var)       ({ asm volatile( "movw %%ss,  %0;" : "=r"(var) ); })
#define M_GR_CS(var)       ({ asm volatile( "movw %%cs,  %0;" : "=r"(var) ); })
#define M_GR_DS(var)       ({ asm volatile( "movw %%ds,  %0;" : "=r"(var) ); })
#define M_GR_ES(var)       ({ asm volatile( "movw %%es,  %0;" : "=r"(var) ); })
#define M_GR_FS(var)       ({ asm volatile( "movw %%fs,  %0;" : "=r"(var) ); })
#define M_GR_GS(var)       ({ asm volatile( "movw %%gs,  %0;" : "=r"(var) ); })

#define M_GR_ESP(var)      ({ asm volatile( "movl %%esp, %0;" : "=r"(var) ); })
#define M_GR_SP(var)       ({ asm volatile( "movw %%sp,  %0;" : "=r"(var) ); })

/* relocated data by multiboot loader */
/*
struct multi_boot_info {
    u32 flags;
    u32 mem_lower_size_kB;
    u32 mem_upper_size_kB;
    char* cmdline_ptr;
    u32 mods_count;
    void* mods_addr;
    void* syms[4];
    u32 mmap_length;
    void* mmap_addr;
    u32 drives_length;
    void* drives_addr;
    void* config_table;
    char* boot_loader_name;
    void* apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
};
*/


int main( void ) {
    kosh_instruction* next_instruction;
    struct multiboot_relocated_info* mri;

    kterm_create();

    do {
        next_instruction = prompt();

        /* should not get _ERROR_ or _EMPTY_ from the call to prompt() */
        switch (next_instruction->command) {
            case ECHO:
                kterm_printf( "%s\n", next_instruction->remaining_command_line );  /* will include inputed newline */
                break;

            case PEEK:
                if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_REG_SET) {
                    u32 regval32;
                    u16 regval16;
                    u8  regval8;

                    switch (next_instruction->reg) {
                        case EAX: M_GR_EAX(regval32); kterm_printf( " eax = 0x%x\n", regval32 ); break;
                        case AX:  M_GR_AX (regval16); kterm_printf( " ax  = 0x%x\n", regval16 ); break;
                        case AL:  M_GR_AL (regval8);  kterm_printf( " al  = 0x%x\n", regval8  ); break;
                        case AH:  M_GR_AH (regval8);  kterm_printf( " ah  = 0x%x\n", regval8  ); break;
                        case SS:  M_GR_SS (regval16); kterm_printf( " ss  = 0x%x\n", regval16 ); break;
                        case CS:  M_GR_CS (regval16); kterm_printf( " cs  = 0x%x\n", regval16 ); break;
                        case DS:  M_GR_DS (regval16); kterm_printf( " ds  = 0x%x\n", regval16 ); break;
                        case ES:  M_GR_ES (regval16); kterm_printf( " es  = 0x%x\n", regval16 ); break;
                        case FS:  M_GR_FS (regval16); kterm_printf( " fs  = 0x%x\n", regval16 ); break;
                        case GS:  M_GR_GS (regval16); kterm_printf( " gs  = 0x%x\n", regval16 ); break;
                        case ESP: M_GR_ESP(regval32); kterm_printf( " esp = 0x%x\n", regval32 ); break;
                        case SP:  M_GR_SP (regval16); kterm_printf( " sp  = 0x%x\n", regval16 ); break;

                        default : kterm_printf( "Code Incomplete For That Register\n" );
                    }
                }
                else if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) {
                    u32 val = *((u32*)(next_instruction->memory_location));
                    kterm_printf( "addr (0x%x) = 0x%x\n", next_instruction->memory_location, val );
                }

                break;

            case POKE:
                kterm_printf( "poke\n" );
                break;

            case DUMPREGS:
                kterm_printf( "dumpregs\n" );
                break;

            case BIOS:
                mri = retrieve_multiboot_relocate_info();
                // for these, really should check mri.flags for each to make sure it is present
                kterm_printf( "lower mem = %i\n", mri->mem_lower_size_kB );
                kterm_printf( "upper mem = %i\n", mri->mem_upper_size_kB );
                kterm_printf( "total mem = %i MB\n", (((mri->mem_lower_size_kB + mri->mem_upper_size_kB) / 1000) + 1) );
                kterm_printf( "cmdline   = %s\n", mri->cmdline_ptr       );
                kterm_printf( "bootdev   = " ); puts_bios_drive_info( mri->boot_device ); kterm_printf( "\n" );
                kterm_printf( "mmap:\n" ); puts_multiboot_mmap_info( mri->mmap_length, mri->mmap_addr ); kterm_printf( "\n" );

//    u32 mods_count;
//    void* mods_addr;
//    void* syms[4];
//    u32 mmap_length;
//    void* mmap_addr;
//    u32 drives_length;
//    void* drives_addr;
//    void* config_table;
//    char* boot_loader_name;
//    void* apm_table;
//    u32 vbe_control_info;
//    u32 vbe_mode_info;
//    u16 vbe_mode;
//    u16 vbe_interface_seg;
//    u16 vbe_interface_off;
//    u16 vbe_interface_len;
                break;

            default:
                break;
        }
    } while (next_instruction->command != EXIT);

    return 0;    
}

