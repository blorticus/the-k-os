[BITS 32]

global cpuid_is_supported      ; int cpuid_is_supported( void ) -- returns 1 if CPUID supported; 0 if not

; This uses a mechanism detailed here: http://wiki.osdev.org/CPUID#How_to_use_CPUID
; I suspect that we could just issue the command 'cpuid' and see if an INVALID_OPCODE
; exception is raised, but this method doesn't rely on exception handlers having been
; configured.  In brief, before CPUID (according to docs above), the ID bit of eflags
; could not be set.  This reads that flag, inverts it, the reads it again.  If it
; is changed, then this return 1; otherwise, it returns 0.
; I believe this routine is interruptable
cpuid_is_supported:
    pushfd                  ; push flags onto stack
    pop eax                 ; copy flags (from stack) into eax
    mov ecx, eax            ; copy same set of flags into ecx
    xor eax, 0x200000       ; flip ID flag position in eax
    push eax                ; push flags w/ flipped bit to stack
    popfd                   ; restore eflags from just pushed stack value (thus setting ID flag)
    pushfd                  ; push flags onto stack
    pop eax                 ; copy flags into eax
    xor eax, ecx            ; ecx has original flags values so if ID flag matches, it'll be 0 after xor; otherwise, it'll be 1
    shr eax, 21             ; ID flag is bit 21; shift it to first bit position
    and eax, 1              ; mask all other flags, leaving value of 0x00000001 or 0x00000000
    ret
