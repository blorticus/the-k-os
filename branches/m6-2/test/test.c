#include <stdio.h>

struct idt_entry {
    unsigned short a;
    unsigned short b;
    unsigned short c;
    unsigned short d;
} __attribute__((packed));


int main( void ) {
    struct idt_entry table[100];
    printf( "idt_entry: %d\ntable: %d\n", sizeof( table ), sizeof( struct idt_entry ) ); 
}
