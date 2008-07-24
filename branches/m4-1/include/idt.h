#ifndef __IDT_H__
#define __IDT_H__

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

#endif
