#include <kosh/koshlib.h>
#include <video/kterm.h>
#include <multiboot.h>
#include <stdio.h>
#include <sys/types.h>
#include <platform/ia-32/cpu.h>
#include <platform/ia-32/interrupts.h>
#include <sys/kernelsyms.h>
#include <memory/paging.h>
#include <memory/kmalloc.h>
#include <bus/pci.h>
#include <platform/ia-32/asm.h>

extern void phys_core_set_window( KTERM_WINDOW w );

#define INPUT_BUFFER_SIZE   100

kterm_window w1, d1, w2;
KTERM_WINDOW top_win     = &w1;
KTERM_WINDOW divider_win = &d1;
KTERM_WINDOW bottom_win  = &w2;

char input_buffer[INPUT_BUFFER_SIZE];

struct kosh_regs {
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


static const char* CPUID_FEATURE_FIXED_LENGTH_NAMES[] = {
     "FPU  ",
     "VME  ",
     "DE   ",
     "PSE  ",
     "TSC  ",
     "MSR  ",
     "PAE  ",
     "MCE  ",
     "CX8  ",
     "APIC ",
     "---- ",
     "SEP  ",
     "MTRR ",
     "PGE  ",
     "MCA  ",
     "CMOV ",
     "PAT  ",
     "PSE36",
     "PSN  ",
     "CLFL ",
     "---- ",
     "DTES ",
     "ACPI ",
     "MMX  ",
     "FXSR ",
     "SSE  ",
     "SSE2 ",
     "SS   ",
     "HTT  ",
     "TM1  ",
     "IA64 ",
     "PBE  "
};




void dumpregs(struct kosh_regs r) {
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


irq_handler_routine ihr = 0;    // for int_diag function

/* for int_diag, replace the irq0 handler with this one.  When it receives irq0, it
 * prints a message to bottom_win, then restores the old handler, stored in
 * the global ihr.  Not thread safe in so, so many ways */
void int_diag_pit_handler( struct regs *r ) {
    kterm_window_printf( bottom_win, "RECEIVED IRQ0\n" );

    kterm_window_printf( bottom_win, "Reinstalling Original Handler...\n" );
    irq_install_handler( 0, ihr );
}


int main( void ) {
    kosh_instruction* next_instruction;
    struct multiboot_relocated_info* mri;
    int i;
    cpuid_retval crv;
    int pos_count;              // for cpuid function

//    u32 *dir, *table, paddr;
//    char *TEST_A, *TEST_B, *t;
//    u32 phys_addr, virt_addr;
//    u32 va;
    char *s1, *cs1, *ct1;
    const char *c1 = "This string (0*!#$) has\n100 characters in\n  --- it! ';{}][,. including the trailing NULL. ... \t9\n";
//    u32* da, *ta;
    pci_scan_iterator psi;
    pci_device pd;
    pci_device* pdp = &pd;
    PCI_SCAN_ITERATOR psip = &psi;

    // kterm MUST BE initialized
    kterm_create_window( top_win,     0,   20, 80 );
    kterm_create_window( divider_win, 1600, 1, 80 );
    kterm_create_window( bottom_win,  1680, 4, 80 );

    kterm_window_cls( top_win );
    kterm_window_cls( divider_win );
    kterm_window_cls( bottom_win );

    fault_handler_set_kterm_window( bottom_win );   /* if processor exception raised, print message in bottom window */

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
                        case EAX: M_GR_EAX(regval32); kterm_window_printf( top_win, " eax = 0x%x\n", regval32 ); break;
                        case EBX: M_GR_EBX(regval32); kterm_window_printf( top_win, " ebx = 0x%x\n", regval32 ); break;
                        case ECX: M_GR_ECX(regval32); kterm_window_printf( top_win, " ecx = 0x%x\n", regval32 ); break;
                        case EDX: M_GR_EDX(regval32); kterm_window_printf( top_win, " edx = 0x%x\n", regval32 ); break;
                        case AX:  M_GR_AX (regval16); kterm_window_printf( top_win, " ax  = 0x%x\n", regval16 ); break;
                        case BX:  M_GR_BX (regval16); kterm_window_printf( top_win, " bx  = 0x%x\n", regval16 ); break;
                        case CX:  M_GR_CX (regval16); kterm_window_printf( top_win, " cx  = 0x%x\n", regval16 ); break;
                        case DX:  M_GR_DX (regval16); kterm_window_printf( top_win, " dx  = 0x%x\n", regval16 ); break;
                        case AL:  M_GR_AL (regval8);  kterm_window_printf( top_win, " al  = 0x%x\n", regval8  ); break;
                        case AH:  M_GR_AH (regval8);  kterm_window_printf( top_win, " ah  = 0x%x\n", regval8  ); break;
                        case SS:  M_GR_SS (regval16); kterm_window_printf( top_win, " ss  = 0x%x\n", regval16 ); break;
                        case CS:  M_GR_CS (regval16); kterm_window_printf( top_win, " cs  = 0x%x\n", regval16 ); break;
                        case DS:  M_GR_DS (regval16); kterm_window_printf( top_win, " ds  = 0x%x\n", regval16 ); break;
                        case ES:  M_GR_ES (regval16); kterm_window_printf( top_win, " es  = 0x%x\n", regval16 ); break;
                        case FS:  M_GR_FS (regval16); kterm_window_printf( top_win, " fs  = 0x%x\n", regval16 ); break;
                        case GS:  M_GR_GS (regval16); kterm_window_printf( top_win, " gs  = 0x%x\n", regval16 ); break;
                        case ESP: M_GR_ESP(regval32); kterm_window_printf( top_win, " esp = 0x%x\n", regval32 ); break;
                        case SP:  M_GR_SP (regval16); kterm_window_printf( top_win, " sp  = 0x%x\n", regval16 ); break;

                        default : kterm_window_printf( top_win, "Code Incomplete For That Register\n" );
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
                kterm_window_printf( top_win, "CR0: %x  CR3: %x\n", read_from_control_register_0(), read_from_control_register_3() );
                break;

            case HELP:
                kterm_window_puts( top_win, " echo <text>       - repeat <text>\n" );
                kterm_window_puts( top_win, " peek <reg|mem>    - see value at register <reg> or memory location 0x<mem>\n" );
                kterm_window_puts( top_win, " poke <reg|mem>    - change value at register <reg> or memory location 0x<mem>\n" );
                kterm_window_puts( top_win, " regs              - dump all register values\n" );
                kterm_window_puts( top_win, " bios              - prints out relocated bios values\n" );
                kterm_window_puts( top_win, " int               - activates interrupt diagnostics\n" );
                kterm_window_puts( top_win, " cpuid             - Show CPUID support and characteristics\n" );
                kterm_window_puts( top_win, " kernel            - Information about the kernel\n" );
                kterm_window_puts( top_win, " kmalloc           - Test kmalloc/kfree implementation\n" );
                kterm_window_puts( top_win, " pci scan          - Scan the PCI bus\n" );
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

            case CPUID:
                if (cpuid_is_supported()) {
                    kterm_window_printf( top_win, "CPUID Supported = true\n" );
                    execute_cpuid_function( CPUID_FUNC_PROC_TYPE_STEPPING_AND_FEATURES, &crv );

                    /* step through bits on crv, and if feature is present print fixed length name with +; otherwise, with a - */
                    for (i = 0, pos_count = 0; i < 32; i++) {
                        if (CPUID_FEATURE_FIXED_LENGTH_NAMES[i][0] != '-') { // there are some reserved bits for cpuid features.  Those are ----- in our static name, so skip them
                            if (crv.edx >> i & 0x00000001)
                                kterm_window_printf( top_win, "+%s ", CPUID_FEATURE_FIXED_LENGTH_NAMES[i] );
                            else
                                kterm_window_printf( top_win, "-%s ", CPUID_FEATURE_FIXED_LENGTH_NAMES[i] );

                            if (++pos_count % 11 == 0)
                                kterm_window_putc( top_win, '\n' );
                        }
                    }
                    kterm_window_putc( top_win, '\n' );
                }
                else {
                    kterm_window_printf( top_win, "CPUID Supported = false\n" );
                }

                break;

            case INTDIAG:
                kterm_window_printf( bottom_win, "WAITING FOR IRQ0 ... " );
                ihr = irq_install_handler( 0, &int_diag_pit_handler );
                break;

            case KERNEL_INFO:
                kterm_window_printf( top_win, "KERNEL START = 0x%x  END = 0x%x\n", (u32)START_OF_KERNEL, (u32)END_OF_KERNEL );
                break;

            case KMALLOC:
                s1 = (char*)kmalloc( 100 );

                if (!s1) {
                    kterm_window_printf( top_win, "s1 kmalloc() returned NULL\n" );
                }
                else {
                    cs1 = s1;
                    ct1 = (char*)c1;

                    while ((*cs1++ = *ct1++)) ;

                    kterm_window_printf( top_win, "STRING 1: vmaddr = 0x%x\n          string = %s\n", (u32)s1, s1 );
                }

                kfree( s1 );

                break;

            case PCI_BUS_SCAN:
                init_pci_scan( psip );

                while (continue_pci_scan( psip, pdp )) {
                    kterm_window_printf( top_win, "- BUS:SLOT = %d:%d VENDOR:DEVICE = 0x%x:0x%x\n  CLASS = %d (%s)\n  SUBCLASS = 0x%x  ProgIF = 0x%x  RevID = 0x%x\n",
                                                  pdp->bus_number, pdp->slot_number, (u32)(pdp->vendor_id), (u32)(pdp->device_id),
                                                  (u32)(pdp->class_code), get_static_pci_class_description( pdp->class_code ),
                                                  (u32)(pdp->subclass), (u32)(pdp->programming_interface), (u32)(pdp->revision_id) );
                }

                if (pdp == NULL) {
                    kterm_window_printf( top_win, "--> ERROR, no match on scan" );
                }
                else {
                }
                
                break;

            case TEST:
//                da = (u32*)0xfffff000;
//                ta = (u32*)0xffc00000;
//
//                kterm_window_printf( top_win, "AFTER: DIR = 0x%x, DIR[0] = 0x%x, DIR[1023] = 0x%x (%d)\n  TA = 0x%x (%d), TA[3] = 0x%x (%d)\n", (u32)da, da[0], da[1023], da[1023], (u32)ta, (u32)ta, ta[3], ta[3] );
//
//                va = allocate_virtual_page( &virt_addr, &phys_addr );
//                kterm_window_printf( top_win, "PHYS: 0x%x, VIRT: 0x%x, VA: 0x%x\n", phys_addr, virt_addr, (u32)va );
//                TEST_A = (char*)va;
//                TEST_B = (char*)"Take it to the limit and beyond";
//                while (*TEST_B) *TEST_A++ = *TEST_B++;
//                kterm_window_printf( top_win, "TEST_A: %s\n", (char*)va );
//
//                va = allocate_virtual_page( &virt_addr, &phys_addr );
//
//                kterm_window_printf( top_win, "PHYS: 0x%x, VIRT: 0x%x, VA: 0x%x\n", phys_addr, virt_addr, (u32)va );
//                TEST_A = (char*)va;
//                TEST_B = (char*)"AND DO IT AGAIN!!!!! AND AGAIN!!!! AND AGAIN!!!!!!!!!!!!!!!!!!!!!!!!";
//                while (*TEST_B) *TEST_A++ = *TEST_B++;
//                kterm_window_printf( top_win, "TEST_A: %s\n", (char*)va );
//
//                va = allocate_virtual_page( &virt_addr, &phys_addr );
//                kterm_window_printf( top_win, "PHYS: 0x%x, VIRT: 0x%x, VA: 0x%x\n", phys_addr, virt_addr, (u32)va );
//                TEST_A = (char*)va;
//                TEST_B = (char*)"Then one more\n  time\n foo";
//                while (*TEST_B) *TEST_A++ = *TEST_B++;
//                kterm_window_printf( top_win, "TEST_A: %s\n", (char*)va );

                break;

            default:
                break;
        }
    } while (next_instruction->command != EXIT);

    return 0;    
}

