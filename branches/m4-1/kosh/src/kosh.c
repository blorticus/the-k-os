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

    kterm_printf( "drive %x [%s], partition %x:%x:%x",
                        drive,
                        (drive == 0x00 ? "fd0" : (drive == 0x01 ? "fd1" : (drive == 0x80 ? "hd0" : (drive == 0x81 ? "hd1" : "unknown")))),
                        (u8)(drive_info >> 16),
                        (u8)(drive_info >> 8), 
                        (u8)(drive_info) );
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
                kterm_puts( next_instruction->remaining_command_line );  /* will include inputed newline */
                kterm_putc( '\n' );
                break;

            case PEEK:
                if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_REG_SET) {
                    u32 regval32;
                    u16 regval16;
                    u8  regval8;

                    switch (next_instruction->reg) {
                        case EAX: M_GR_EAX(regval32); kterm_puts( " eax = " ); kterm_puth( regval32 ); kterm_putc( '\n' ); break;
                        case AX:  M_GR_AX (regval16); kterm_puts( " ax  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case AL:  M_GR_AL (regval8);  kterm_puts( " al  = " ); kterm_puth( regval8  ); kterm_putc( '\n' ); break;
                        case AH:  M_GR_AH (regval8);  kterm_puts( " ah  = " ); kterm_puth( regval8  ); kterm_putc( '\n' ); break;
                        case SS:  M_GR_SS (regval16); kterm_puts( " ss  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case CS:  M_GR_CS (regval16); kterm_puts( " cs  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case DS:  M_GR_DS (regval16); kterm_puts( " ds  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case ES:  M_GR_ES (regval16); kterm_puts( " es  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case FS:  M_GR_FS (regval16); kterm_puts( " fs  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case GS:  M_GR_GS (regval16); kterm_puts( " gs  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;
                        case ESP: M_GR_ESP(regval32); kterm_puts( " esp = " ); kterm_puth( regval32 ); kterm_putc( '\n' ); break;
                        case SP:  M_GR_SP (regval16); kterm_puts( " sp  = " ); kterm_puth( regval16 ); kterm_putc( '\n' ); break;

                        default : kterm_puts( "Code Incomplete For That Register\n" );
                    }
                }
                else if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) {
                    u32 val = *((u32*)(next_instruction->memory_location));
                    kterm_puts( "addr (" ); kterm_puth( next_instruction->memory_location ); kterm_puts( ") = " ); kterm_puth( val ); kterm_putc( '\n' );
                }

                break;

            case POKE:
                kterm_puts( "poke\n" );
                break;

            case DUMPREGS:
                kterm_puts( "dumpregs\n" );
                break;

            case BIOS:
                mri = retrieve_multiboot_relocate_info();
                kterm_printf( "lower mem = %i\n", mri->mem_lower_size_kB );
                kterm_printf( "upper mem = %i\n", mri->mem_upper_size_kB );
                kterm_printf( "total mem = %i MB\n", (((mri->mem_lower_size_kB + mri->mem_upper_size_kB) / 1000) + 1) );
                kterm_printf( "cmdline   = %s\n", mri->cmdline_ptr       );
                kterm_printf( "bootdev   = " ); puts_bios_drive_info( mri->boot_device ); kterm_printf( "\n" );

//                kterm_puts( "lower mem = " ); kterm_puti( mri->mem_lower_size_kB ); kterm_putc( '\n' );
//                kterm_puts( "upper mem = " ); kterm_puti( mri->mem_upper_size_kB ); kterm_putc( '\n' );
//                kterm_puts( "cmdline   = " ); kterm_puts( mri->cmdline_ptr       ); kterm_putc( '\n' );
//                kterm_puts( "bootdev   = " ); puts_bios_drive_info( mri->boot_device ); kterm_putc( '\n' );
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
