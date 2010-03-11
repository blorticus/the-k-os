#include <kosh/koshlib.h>
#include <video/kterm.h>
#include <multiboot.h>
#include <stdio.h>
#include <sys/types.h>

#define INPUT_BUFFER_SIZE   100

kterm_window w1, d1, w2;
KTERM_WINDOW top_win     = &w1;
KTERM_WINDOW divider_win = &d1;
KTERM_WINDOW bottom_win  = &w2;

char input_buffer[INPUT_BUFFER_SIZE];

struct regs {
    // EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u16 gs;
    u16 fs;
    u16 es;
    u16 ds;
    u16 ss;
}__attribute__((packed));

void dumpregs(struct regs r) {
    kterm_window_printf( top_win, "eax = %8x    ebx = %8x    ecx = %8x    edx = %8x\nesp = %8x    ebp = %8x    esi = %8x    edi = %8x\nss  = %8x    ds  = %8x    es  = %8x    fs  = %8x\ngs  = %8x\n",
                         r.eax, r.ebx, r.ecx, r.edx, r.esp, r.ebp, r.esi, r.edi, r.ss, r.ds, r.es, r.fs, r.gs );
}

kosh_instruction* prompt( void ) {
    kosh_instruction* instruction;

    for ( ;; ) {
        kterm_window_printf( top_win, "KoSH> " );
        kterm_window_readline( top_win, input_buffer, INPUT_BUFFER_SIZE );
        instruction = input_to_instruction( input_buffer );

        if (instruction->command == _ERROR_) {
            kterm_window_printf( top_win, "ERROR: %s\n", instruction->error );
        }
        else if (instruction->command == _EMPTY_) {
            kterm_window_printf( top_win, "_EMPTY_\n" );
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

    kterm_window_printf( top_win, "drive %x [%s], partition %x:%x:%x",
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


int main( void ) {
    kosh_instruction* next_instruction;
    struct multiboot_relocated_info* mri;
    int i;

    // kterm MUST BE initialized
//    kterm_dup_root_window( top_win );
    kterm_create_window( top_win,     0,   20, 80 );
    kterm_create_window( divider_win, 1600, 1, 80 );
    kterm_create_window( bottom_win,  1680, 4, 80 );

    kterm_window_cls( top_win );
    kterm_window_cls( divider_win );
    kterm_window_cls( bottom_win );

    for (i = 0; i < 80; i++)
        kterm_window_putc( divider_win, '=' );

    do {
        next_instruction = prompt();

        /* should not get _ERROR_ or _EMPTY_ from the call to prompt() */
        switch (next_instruction->command) {
            case ECHO:
                kterm_window_puts( top_win, next_instruction->remaining_command_line );  /* will include inputed newline */
                kterm_window_putc( top_win, '\n' );
                break;

            case PEEK:
                if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_REG_SET) {
                    u32 regval32;
                    u16 regval16;
                    u8  regval8;

                    switch (next_instruction->reg) {
                        case EAX: M_GR_EAX(regval32); kterm_window_puts( top_win, " eax = " ); kterm_window_puth( top_win, regval32 ); kterm_window_putc( top_win, '\n' ); break;
                        case EBX: M_GR_EBX(regval32); kterm_window_puts( top_win, " ebx = " ); kterm_window_puth( top_win, regval32 ); kterm_window_putc( top_win, '\n' ); break;
                        case ECX: M_GR_ECX(regval32); kterm_window_puts( top_win, " ecx = " ); kterm_window_puth( top_win, regval32 ); kterm_window_putc( top_win, '\n' ); break;
                        case EDX: M_GR_EDX(regval32); kterm_window_puts( top_win, " edx = " ); kterm_window_puth( top_win, regval32 ); kterm_window_putc( top_win, '\n' ); break;
                        case AX:  M_GR_AX (regval16); kterm_window_puts( top_win, " ax  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case BX:  M_GR_BX (regval16); kterm_window_puts( top_win, " bx  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case CX:  M_GR_CX (regval16); kterm_window_puts( top_win, " cx  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case DX:  M_GR_DX (regval16); kterm_window_puts( top_win, " dx  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case AL:  M_GR_AL (regval8);  kterm_window_puts( top_win, " al  = " ); kterm_window_puth( top_win, regval8  ); kterm_window_putc( top_win, '\n' ); break;
                        case AH:  M_GR_AH (regval8);  kterm_window_puts( top_win, " ah  = " ); kterm_window_puth( top_win, regval8  ); kterm_window_putc( top_win, '\n' ); break;
                        case SS:  M_GR_SS (regval16); kterm_window_puts( top_win, " ss  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case CS:  M_GR_CS (regval16); kterm_window_puts( top_win, " cs  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case DS:  M_GR_DS (regval16); kterm_window_puts( top_win, " ds  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case ES:  M_GR_ES (regval16); kterm_window_puts( top_win, " es  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case FS:  M_GR_FS (regval16); kterm_window_puts( top_win, " fs  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case GS:  M_GR_GS (regval16); kterm_window_puts( top_win, " gs  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;
                        case ESP: M_GR_ESP(regval32); kterm_window_puts( top_win, " esp = " ); kterm_window_puth( top_win, regval32 ); kterm_window_putc( top_win, '\n' ); break;
                        case SP:  M_GR_SP (regval16); kterm_window_puts( top_win, " sp  = " ); kterm_window_puth( top_win, regval16 ); kterm_window_putc( top_win, '\n' ); break;

                        default : kterm_window_puts( top_win, "Code Incomplete For That Register\n" );
                    }
                }
                else if (next_instruction->flags & KOSH_INSTRUCTION_FLAG_MEMADDR_SET) {
                    u32 val = *((u32*)(next_instruction->memory_location));
                    kterm_window_puts( top_win, "addr (" ); kterm_window_puth( top_win, next_instruction->memory_location ); kterm_window_puts( top_win, ") = " ); kterm_window_puth( top_win, val ); kterm_window_putc( top_win, '\n' );
                }

                break;

            case POKE:
                kterm_window_puts( top_win, "poke\n" );
                break;

            case DUMPREGS:
                asm("pushw %ss");
                asm("pushw %ds");
                asm("pushw %es");
                asm("pushw %fs");
                asm("pushw %gs");
                asm("pusha");
                typedef void (*fp)(void);
                fp f;
                f = (fp) dumpregs;
                f();
                break;

            case HELP:
                kterm_window_puts( top_win, " echo <text>       - repeat <text>\n" );
                kterm_window_puts( top_win, " peek <reg|mem>    - see value at register <reg> or memory location 0x<mem>\n" );
                kterm_window_puts( top_win, " poke <reg|mem>    - change value at register <reg> or memory location 0x<mem>\n" );
                kterm_window_puts( top_win, " regs              - dump all register values\n" );
                kterm_window_puts( top_win, " bios              - prints out relocated bios values\n" );
                kterm_window_puts( top_win, " int               - activates interrupt diagnostics\n" );
                break;

            case BIOS:
                mri = retrieve_multiboot_relocate_info();
                kterm_window_printf( top_win, "lower mem = %i\n", mri->mem_lower_size_kB );
                kterm_window_printf( top_win, "upper mem = %i\n", mri->mem_upper_size_kB );
                kterm_window_printf( top_win, "total mem = %i MB\n", (((mri->mem_lower_size_kB + mri->mem_upper_size_kB) / 1000) + 1) );
                kterm_window_printf( top_win, "cmdline   = %s\n", mri->cmdline_ptr       );
                kterm_window_printf( top_win, "bootdev   = " ); puts_bios_drive_info( mri->boot_device ); kterm_window_printf( top_win, "\n" );
                kterm_window_printf( top_win, "length of mmap entries = %d bytes starting at %x\n", mri->mmap_length, mri->mmap_addr );
                struct multiboot_mmap_entry* next_mmap_entry = (struct multiboot_mmap_entry*)mri->mmap_addr;
                u32 traversed = 0;

                if (mri->mmap_length) {
                      while (traversed < mri->mmap_length) {
                        kterm_window_printf( top_win, "  -- size = %d, base_addr = %x:%x, length = %d:%d, type = %d\n",
                                      next_mmap_entry->entry_size, next_mmap_entry->base_addr_high, next_mmap_entry->base_addr_low,
                                      next_mmap_entry->length_high, next_mmap_entry->length_low, next_mmap_entry->type );
                        traversed += next_mmap_entry->entry_size + 4;   // +4 because of the size, which isn't part of entry
                        next_mmap_entry = (struct multiboot_mmap_entry*)((u8*)next_mmap_entry + next_mmap_entry->entry_size + 4);
                      }
                }

                break;

            case INTDIAG:
                kterm_window_puts( top_win, "\nSTART DIAG ... " );
                break;

            default:
                break;
        }
    } while (next_instruction->command != EXIT);

    return 0;    
}

