#include <Interrupts.h>
#include <String.h>
#include <IOPorts.h>

#define PIC_ICW1_IC4        0x01
#define PIC_ICW1_INIT       0x01
#define PIC_ICW3_ID0_IR2    0x04
#define PIC_ICW4_IS_8086    0x01

#define PIC1_COMMAND_PORT   0x20
#define PIC1_DATA_PORT      0x21
#define PIC2_COMMAND_PORT   0xA0
#define PIC2_DATA_PORT      0xA1


static InterruptDescriptorTableRegister_t staticInterruptDescriptorTableRegister;
static InterruptDescriptor_t staticDefaultInterruptDescriptors[256];
static InterruptDescriptorTable_t staticDefaultInterruptTable = {
    .Descriptors = staticDefaultInterruptDescriptors,
    .NumberOfDescriptorsInTable = 256,
};
static InterruptVectorCallback staticDefaultTableInterruptVectorCallbacks[256];

Error PopulateInterruptDescriptor( InterruptDescriptor d, void* handlerAddress, CpuPrivilegeLevel cpl, int isTrapGate ) {
    d->CodeSegementSelector = 40;   // Stivale2 places 64-bit code GDT entry at slot 5
    d->PrivilegeLevelFlagsAndType = 0x80 | (cpl << 7) | (isTrapGate ? 0xf : 0xe);
    d->HandlerAddressLow = (uint16_t)((uint64_t)handlerAddress);
    d->HandlerAddressMid = (uint16_t)((uint64_t)handlerAddress >> 16);
    d->HandlerAddressHigh = (uint32_t)((uint64_t)handlerAddress >> 32);
    d->InterruptStackTable = 0;
    d->Reserved = 0;

    return NoError;
}

static void ActivateTable()
{
    staticInterruptDescriptorTableRegister.AddressOfFirstEntry = (uint64_t)(staticDefaultInterruptTable.Descriptors);
    staticInterruptDescriptorTableRegister.ByteCountForAllDescriptorsInTable = staticDefaultInterruptTable.NumberOfDescriptorsInTable * sizeof(InterruptDescriptor_t);
    asm volatile ( "lidt %0" : : "m" (staticInterruptDescriptorTableRegister) );
}

static Error SetInterruptVectorCallback( uint8_t interruptNumber, InterruptVectorCallback callback )
{
    staticDefaultTableInterruptVectorCallbacks[interruptNumber] = callback;
    return NoError;
}

typedef struct InterruptFrame_t *InterruptFrame;

static GenericGlyphStringOutputter defaultGlyphStringOutputter;

RuneString StandardInterruptNames[] = {
    U"Divide By Zero Error",
    U"Debug",
    U"Non-Maskable Interrupt",
    U"Breakpoint",
    U"Overflow",
    U"Bound Range",
    U"Invalid Opcode",
    U"Device Not Available",
    U"Double Fault",
    U"Reserved",
    U"Invalid TSS",
    U"Segement Not Present",
    U"Stack",
    U"General Protection Fault",
    U"Page Fault",
    U"Reserved",
    U"Floating Point Exception Pending",
    U"Alignment Check",
    U"Machine Check",
    U"SIMD Floating Point",
    U"Reserved",
    U"Control-Protection Exception",
    U"Reserved",
    U"Reserved",
    U"Reserved",
    U"Reserved",
    U"Reserved",
    U"Reserved",
    U"Hypervisor Injection Exception",
    U"VMM Communication Exception",
    U"Security Exception",
    U"Reserved"
};

static Rune runeBuffer[256];
static RuneStringBuffer_t runeStringBuffer = {
    .String = runeBuffer,
    .Size = 256,
};

static void staticDefaultInterruptRoutine( uint8_t interruptNumber ) {
    defaultGlyphStringOutputter( U"ISR Routine for " );
    if (interruptNumber < 32) 
        defaultGlyphStringOutputter( StandardInterruptNames[interruptNumber] );
    else {
        Uint64ToDecimalString( &runeStringBuffer, interruptNumber );
        defaultGlyphStringOutputter( runeStringBuffer.String );
    }
    defaultGlyphStringOutputter( U"\n" );
}

static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int0( InterruptFrame frame ) { staticDefaultInterruptRoutine( 0 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int1( InterruptFrame frame ) { staticDefaultInterruptRoutine( 1 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int2( InterruptFrame frame ) { staticDefaultInterruptRoutine( 2 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int3( InterruptFrame frame ) { staticDefaultInterruptRoutine( 3 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int4( InterruptFrame frame ) { staticDefaultInterruptRoutine( 4 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int5( InterruptFrame frame ) { staticDefaultInterruptRoutine( 5 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int6( InterruptFrame frame ) { staticDefaultInterruptRoutine( 6 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int7( InterruptFrame frame ) { staticDefaultInterruptRoutine( 7 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int8( InterruptFrame frame ) { staticDefaultInterruptRoutine( 8 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int9( InterruptFrame frame ) { staticDefaultInterruptRoutine( 9 ); }   
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int10( InterruptFrame frame ) { staticDefaultInterruptRoutine( 10 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int11( InterruptFrame frame ) { staticDefaultInterruptRoutine( 11 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int12( InterruptFrame frame ) { staticDefaultInterruptRoutine( 12 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int13( InterruptFrame frame ) { staticDefaultInterruptRoutine( 13 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int14( InterruptFrame frame ) { staticDefaultInterruptRoutine( 14 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int15( InterruptFrame frame ) { staticDefaultInterruptRoutine( 15 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int16( InterruptFrame frame ) { staticDefaultInterruptRoutine( 16 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int17( InterruptFrame frame ) { staticDefaultInterruptRoutine( 17 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int18( InterruptFrame frame ) { staticDefaultInterruptRoutine( 18 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int19( InterruptFrame frame ) { staticDefaultInterruptRoutine( 19 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int20( InterruptFrame frame ) { staticDefaultInterruptRoutine( 20 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int21( InterruptFrame frame ) { staticDefaultInterruptRoutine( 21 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int22( InterruptFrame frame ) { staticDefaultInterruptRoutine( 22 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int23( InterruptFrame frame ) { staticDefaultInterruptRoutine( 23 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int24( InterruptFrame frame ) { staticDefaultInterruptRoutine( 24 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int25( InterruptFrame frame ) { staticDefaultInterruptRoutine( 25 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int26( InterruptFrame frame ) { staticDefaultInterruptRoutine( 26 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int27( InterruptFrame frame ) { staticDefaultInterruptRoutine( 27 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int28( InterruptFrame frame ) { staticDefaultInterruptRoutine( 28 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int29( InterruptFrame frame ) { staticDefaultInterruptRoutine( 29 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int30( InterruptFrame frame ) { staticDefaultInterruptRoutine( 30 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int31( InterruptFrame frame ) { staticDefaultInterruptRoutine( 31 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int32( InterruptFrame frame ) { staticDefaultInterruptRoutine( 32 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int33( InterruptFrame frame ) { staticDefaultInterruptRoutine( 33 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int34( InterruptFrame frame ) { staticDefaultInterruptRoutine( 34 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int35( InterruptFrame frame ) { staticDefaultInterruptRoutine( 35 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int36( InterruptFrame frame ) { staticDefaultInterruptRoutine( 36 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int37( InterruptFrame frame ) { staticDefaultInterruptRoutine( 37 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int38( InterruptFrame frame ) { staticDefaultInterruptRoutine( 38 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int39( InterruptFrame frame ) { staticDefaultInterruptRoutine( 39 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int40( InterruptFrame frame ) { staticDefaultInterruptRoutine( 40 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int41( InterruptFrame frame ) { staticDefaultInterruptRoutine( 41 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int42( InterruptFrame frame ) { staticDefaultInterruptRoutine( 42 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int43( InterruptFrame frame ) { staticDefaultInterruptRoutine( 43 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int44( InterruptFrame frame ) { staticDefaultInterruptRoutine( 44 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int45( InterruptFrame frame ) { staticDefaultInterruptRoutine( 45 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int46( InterruptFrame frame ) { staticDefaultInterruptRoutine( 46 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int47( InterruptFrame frame ) { staticDefaultInterruptRoutine( 47 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int48( InterruptFrame frame ) { staticDefaultInterruptRoutine( 48 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int49( InterruptFrame frame ) { staticDefaultInterruptRoutine( 49 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int50( InterruptFrame frame ) { staticDefaultInterruptRoutine( 50 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int51( InterruptFrame frame ) { staticDefaultInterruptRoutine( 51 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int52( InterruptFrame frame ) { staticDefaultInterruptRoutine( 52 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int53( InterruptFrame frame ) { staticDefaultInterruptRoutine( 53 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int54( InterruptFrame frame ) { staticDefaultInterruptRoutine( 54 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int55( InterruptFrame frame ) { staticDefaultInterruptRoutine( 55 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int56( InterruptFrame frame ) { staticDefaultInterruptRoutine( 56 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int57( InterruptFrame frame ) { staticDefaultInterruptRoutine( 57 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int58( InterruptFrame frame ) { staticDefaultInterruptRoutine( 58 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int59( InterruptFrame frame ) { staticDefaultInterruptRoutine( 59 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int60( InterruptFrame frame ) { staticDefaultInterruptRoutine( 60 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int61( InterruptFrame frame ) { staticDefaultInterruptRoutine( 61 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int62( InterruptFrame frame ) { staticDefaultInterruptRoutine( 62 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int63( InterruptFrame frame ) { staticDefaultInterruptRoutine( 63 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int64( InterruptFrame frame ) { staticDefaultInterruptRoutine( 64 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int65( InterruptFrame frame ) { staticDefaultInterruptRoutine( 65 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int66( InterruptFrame frame ) { staticDefaultInterruptRoutine( 66 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int67( InterruptFrame frame ) { staticDefaultInterruptRoutine( 67 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int68( InterruptFrame frame ) { staticDefaultInterruptRoutine( 68 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int69( InterruptFrame frame ) { staticDefaultInterruptRoutine( 69 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int70( InterruptFrame frame ) { staticDefaultInterruptRoutine( 70 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int71( InterruptFrame frame ) { staticDefaultInterruptRoutine( 71 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int72( InterruptFrame frame ) { staticDefaultInterruptRoutine( 72 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int73( InterruptFrame frame ) { staticDefaultInterruptRoutine( 73 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int74( InterruptFrame frame ) { staticDefaultInterruptRoutine( 74 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int75( InterruptFrame frame ) { staticDefaultInterruptRoutine( 75 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int76( InterruptFrame frame ) { staticDefaultInterruptRoutine( 76 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int77( InterruptFrame frame ) { staticDefaultInterruptRoutine( 77 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int78( InterruptFrame frame ) { staticDefaultInterruptRoutine( 78 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int79( InterruptFrame frame ) { staticDefaultInterruptRoutine( 79 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int80( InterruptFrame frame ) { staticDefaultInterruptRoutine( 80 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int81( InterruptFrame frame ) { staticDefaultInterruptRoutine( 81 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int82( InterruptFrame frame ) { staticDefaultInterruptRoutine( 82 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int83( InterruptFrame frame ) { staticDefaultInterruptRoutine( 83 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int84( InterruptFrame frame ) { staticDefaultInterruptRoutine( 84 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int85( InterruptFrame frame ) { staticDefaultInterruptRoutine( 85 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int86( InterruptFrame frame ) { staticDefaultInterruptRoutine( 86 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int87( InterruptFrame frame ) { staticDefaultInterruptRoutine( 87 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int88( InterruptFrame frame ) { staticDefaultInterruptRoutine( 88 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int89( InterruptFrame frame ) { staticDefaultInterruptRoutine( 89 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int90( InterruptFrame frame ) { staticDefaultInterruptRoutine( 90 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int91( InterruptFrame frame ) { staticDefaultInterruptRoutine( 91 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int92( InterruptFrame frame ) { staticDefaultInterruptRoutine( 92 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int93( InterruptFrame frame ) { staticDefaultInterruptRoutine( 93 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int94( InterruptFrame frame ) { staticDefaultInterruptRoutine( 94 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int95( InterruptFrame frame ) { staticDefaultInterruptRoutine( 95 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int96( InterruptFrame frame ) { staticDefaultInterruptRoutine( 96 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int97( InterruptFrame frame ) { staticDefaultInterruptRoutine( 97 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int98( InterruptFrame frame ) { staticDefaultInterruptRoutine( 98 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int99( InterruptFrame frame ) { staticDefaultInterruptRoutine( 99 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int100( InterruptFrame frame ) { staticDefaultInterruptRoutine( 100 ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int101( InterruptFrame frame ) { staticDefaultInterruptRoutine( 101 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int102( InterruptFrame frame ) { staticDefaultInterruptRoutine( 102 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int103( InterruptFrame frame ) { staticDefaultInterruptRoutine( 103 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int104( InterruptFrame frame ) { staticDefaultInterruptRoutine( 104 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int105( InterruptFrame frame ) { staticDefaultInterruptRoutine( 105 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int106( InterruptFrame frame ) { staticDefaultInterruptRoutine( 106 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int107( InterruptFrame frame ) { staticDefaultInterruptRoutine( 107 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int108( InterruptFrame frame ) { staticDefaultInterruptRoutine( 108 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int109( InterruptFrame frame ) { staticDefaultInterruptRoutine( 109 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int110( InterruptFrame frame ) { staticDefaultInterruptRoutine( 110 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int111( InterruptFrame frame ) { staticDefaultInterruptRoutine( 111 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int112( InterruptFrame frame ) { staticDefaultInterruptRoutine( 112 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int113( InterruptFrame frame ) { staticDefaultInterruptRoutine( 113 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int114( InterruptFrame frame ) { staticDefaultInterruptRoutine( 114 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int115( InterruptFrame frame ) { staticDefaultInterruptRoutine( 115 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int116( InterruptFrame frame ) { staticDefaultInterruptRoutine( 116 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int117( InterruptFrame frame ) { staticDefaultInterruptRoutine( 117 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int118( InterruptFrame frame ) { staticDefaultInterruptRoutine( 118 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int119( InterruptFrame frame ) { staticDefaultInterruptRoutine( 119 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int120( InterruptFrame frame ) { staticDefaultInterruptRoutine( 120 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int121( InterruptFrame frame ) { staticDefaultInterruptRoutine( 121 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int122( InterruptFrame frame ) { staticDefaultInterruptRoutine( 122 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int123( InterruptFrame frame ) { staticDefaultInterruptRoutine( 123 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int124( InterruptFrame frame ) { staticDefaultInterruptRoutine( 124 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int125( InterruptFrame frame ) { staticDefaultInterruptRoutine( 125 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int126( InterruptFrame frame ) { staticDefaultInterruptRoutine( 126 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int127( InterruptFrame frame ) { staticDefaultInterruptRoutine( 127 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int128( InterruptFrame frame ) { staticDefaultInterruptRoutine( 128 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int129( InterruptFrame frame ) { staticDefaultInterruptRoutine( 129 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int130( InterruptFrame frame ) { staticDefaultInterruptRoutine( 130 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int131( InterruptFrame frame ) { staticDefaultInterruptRoutine( 131 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int132( InterruptFrame frame ) { staticDefaultInterruptRoutine( 132 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int133( InterruptFrame frame ) { staticDefaultInterruptRoutine( 133 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int134( InterruptFrame frame ) { staticDefaultInterruptRoutine( 134 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int135( InterruptFrame frame ) { staticDefaultInterruptRoutine( 135 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int136( InterruptFrame frame ) { staticDefaultInterruptRoutine( 136 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int137( InterruptFrame frame ) { staticDefaultInterruptRoutine( 137 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int138( InterruptFrame frame ) { staticDefaultInterruptRoutine( 138 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int139( InterruptFrame frame ) { staticDefaultInterruptRoutine( 139 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int140( InterruptFrame frame ) { staticDefaultInterruptRoutine( 140 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int141( InterruptFrame frame ) { staticDefaultInterruptRoutine( 141 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int142( InterruptFrame frame ) { staticDefaultInterruptRoutine( 142 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int143( InterruptFrame frame ) { staticDefaultInterruptRoutine( 143 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int144( InterruptFrame frame ) { staticDefaultInterruptRoutine( 144 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int145( InterruptFrame frame ) { staticDefaultInterruptRoutine( 145 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int146( InterruptFrame frame ) { staticDefaultInterruptRoutine( 146 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int147( InterruptFrame frame ) { staticDefaultInterruptRoutine( 147 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int148( InterruptFrame frame ) { staticDefaultInterruptRoutine( 148 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int149( InterruptFrame frame ) { staticDefaultInterruptRoutine( 149 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int150( InterruptFrame frame ) { staticDefaultInterruptRoutine( 150 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int151( InterruptFrame frame ) { staticDefaultInterruptRoutine( 151 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int152( InterruptFrame frame ) { staticDefaultInterruptRoutine( 152 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int153( InterruptFrame frame ) { staticDefaultInterruptRoutine( 153 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int154( InterruptFrame frame ) { staticDefaultInterruptRoutine( 154 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int155( InterruptFrame frame ) { staticDefaultInterruptRoutine( 155 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int156( InterruptFrame frame ) { staticDefaultInterruptRoutine( 156 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int157( InterruptFrame frame ) { staticDefaultInterruptRoutine( 157 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int158( InterruptFrame frame ) { staticDefaultInterruptRoutine( 158 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int159( InterruptFrame frame ) { staticDefaultInterruptRoutine( 159 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int160( InterruptFrame frame ) { staticDefaultInterruptRoutine( 160 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int161( InterruptFrame frame ) { staticDefaultInterruptRoutine( 161 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int162( InterruptFrame frame ) { staticDefaultInterruptRoutine( 162 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int163( InterruptFrame frame ) { staticDefaultInterruptRoutine( 163 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int164( InterruptFrame frame ) { staticDefaultInterruptRoutine( 164 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int165( InterruptFrame frame ) { staticDefaultInterruptRoutine( 165 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int166( InterruptFrame frame ) { staticDefaultInterruptRoutine( 166 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int167( InterruptFrame frame ) { staticDefaultInterruptRoutine( 167 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int168( InterruptFrame frame ) { staticDefaultInterruptRoutine( 168 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int169( InterruptFrame frame ) { staticDefaultInterruptRoutine( 169 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int170( InterruptFrame frame ) { staticDefaultInterruptRoutine( 170 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int171( InterruptFrame frame ) { staticDefaultInterruptRoutine( 171 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int172( InterruptFrame frame ) { staticDefaultInterruptRoutine( 172 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int173( InterruptFrame frame ) { staticDefaultInterruptRoutine( 173 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int174( InterruptFrame frame ) { staticDefaultInterruptRoutine( 174 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int175( InterruptFrame frame ) { staticDefaultInterruptRoutine( 175 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int176( InterruptFrame frame ) { staticDefaultInterruptRoutine( 176 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int177( InterruptFrame frame ) { staticDefaultInterruptRoutine( 177 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int178( InterruptFrame frame ) { staticDefaultInterruptRoutine( 178 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int179( InterruptFrame frame ) { staticDefaultInterruptRoutine( 179 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int180( InterruptFrame frame ) { staticDefaultInterruptRoutine( 180 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int181( InterruptFrame frame ) { staticDefaultInterruptRoutine( 181 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int182( InterruptFrame frame ) { staticDefaultInterruptRoutine( 182 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int183( InterruptFrame frame ) { staticDefaultInterruptRoutine( 183 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int184( InterruptFrame frame ) { staticDefaultInterruptRoutine( 184 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int185( InterruptFrame frame ) { staticDefaultInterruptRoutine( 185 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int186( InterruptFrame frame ) { staticDefaultInterruptRoutine( 186 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int187( InterruptFrame frame ) { staticDefaultInterruptRoutine( 187 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int188( InterruptFrame frame ) { staticDefaultInterruptRoutine( 188 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int189( InterruptFrame frame ) { staticDefaultInterruptRoutine( 189 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int190( InterruptFrame frame ) { staticDefaultInterruptRoutine( 190 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int191( InterruptFrame frame ) { staticDefaultInterruptRoutine( 191 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int192( InterruptFrame frame ) { staticDefaultInterruptRoutine( 192 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int193( InterruptFrame frame ) { staticDefaultInterruptRoutine( 193 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int194( InterruptFrame frame ) { staticDefaultInterruptRoutine( 194 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int195( InterruptFrame frame ) { staticDefaultInterruptRoutine( 195 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int196( InterruptFrame frame ) { staticDefaultInterruptRoutine( 196 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int197( InterruptFrame frame ) { staticDefaultInterruptRoutine( 197 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int198( InterruptFrame frame ) { staticDefaultInterruptRoutine( 198 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int199( InterruptFrame frame ) { staticDefaultInterruptRoutine( 199 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int200( InterruptFrame frame ) { staticDefaultInterruptRoutine( 200 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int201( InterruptFrame frame ) { staticDefaultInterruptRoutine( 201 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int202( InterruptFrame frame ) { staticDefaultInterruptRoutine( 202 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int203( InterruptFrame frame ) { staticDefaultInterruptRoutine( 203 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int204( InterruptFrame frame ) { staticDefaultInterruptRoutine( 204 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int205( InterruptFrame frame ) { staticDefaultInterruptRoutine( 205 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int206( InterruptFrame frame ) { staticDefaultInterruptRoutine( 206 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int207( InterruptFrame frame ) { staticDefaultInterruptRoutine( 207 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int208( InterruptFrame frame ) { staticDefaultInterruptRoutine( 208 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int209( InterruptFrame frame ) { staticDefaultInterruptRoutine( 209 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int210( InterruptFrame frame ) { staticDefaultInterruptRoutine( 210 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int211( InterruptFrame frame ) { staticDefaultInterruptRoutine( 211 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int212( InterruptFrame frame ) { staticDefaultInterruptRoutine( 212 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int213( InterruptFrame frame ) { staticDefaultInterruptRoutine( 213 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int214( InterruptFrame frame ) { staticDefaultInterruptRoutine( 214 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int215( InterruptFrame frame ) { staticDefaultInterruptRoutine( 215 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int216( InterruptFrame frame ) { staticDefaultInterruptRoutine( 216 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int217( InterruptFrame frame ) { staticDefaultInterruptRoutine( 217 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int218( InterruptFrame frame ) { staticDefaultInterruptRoutine( 218 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int219( InterruptFrame frame ) { staticDefaultInterruptRoutine( 219 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int220( InterruptFrame frame ) { staticDefaultInterruptRoutine( 220 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int221( InterruptFrame frame ) { staticDefaultInterruptRoutine( 221 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int222( InterruptFrame frame ) { staticDefaultInterruptRoutine( 222 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int223( InterruptFrame frame ) { staticDefaultInterruptRoutine( 223 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int224( InterruptFrame frame ) { staticDefaultInterruptRoutine( 224 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int225( InterruptFrame frame ) { staticDefaultInterruptRoutine( 225 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int226( InterruptFrame frame ) { staticDefaultInterruptRoutine( 226 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int227( InterruptFrame frame ) { staticDefaultInterruptRoutine( 227 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int228( InterruptFrame frame ) { staticDefaultInterruptRoutine( 228 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int229( InterruptFrame frame ) { staticDefaultInterruptRoutine( 229 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int230( InterruptFrame frame ) { staticDefaultInterruptRoutine( 230 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int231( InterruptFrame frame ) { staticDefaultInterruptRoutine( 231 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int232( InterruptFrame frame ) { staticDefaultInterruptRoutine( 232 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int233( InterruptFrame frame ) { staticDefaultInterruptRoutine( 233 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int234( InterruptFrame frame ) { staticDefaultInterruptRoutine( 234 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int235( InterruptFrame frame ) { staticDefaultInterruptRoutine( 235 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int236( InterruptFrame frame ) { staticDefaultInterruptRoutine( 236 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int237( InterruptFrame frame ) { staticDefaultInterruptRoutine( 237 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int238( InterruptFrame frame ) { staticDefaultInterruptRoutine( 238 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int239( InterruptFrame frame ) { staticDefaultInterruptRoutine( 239 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int240( InterruptFrame frame ) { staticDefaultInterruptRoutine( 240 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int241( InterruptFrame frame ) { staticDefaultInterruptRoutine( 241 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int242( InterruptFrame frame ) { staticDefaultInterruptRoutine( 242 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int243( InterruptFrame frame ) { staticDefaultInterruptRoutine( 243 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int244( InterruptFrame frame ) { staticDefaultInterruptRoutine( 244 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int245( InterruptFrame frame ) { staticDefaultInterruptRoutine( 245 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int246( InterruptFrame frame ) { staticDefaultInterruptRoutine( 246 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int247( InterruptFrame frame ) { staticDefaultInterruptRoutine( 247 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int248( InterruptFrame frame ) { staticDefaultInterruptRoutine( 248 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int249( InterruptFrame frame ) { staticDefaultInterruptRoutine( 249 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int250( InterruptFrame frame ) { staticDefaultInterruptRoutine( 250 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int251( InterruptFrame frame ) { staticDefaultInterruptRoutine( 251 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int252( InterruptFrame frame ) { staticDefaultInterruptRoutine( 252 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int253( InterruptFrame frame ) { staticDefaultInterruptRoutine( 253 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int254( InterruptFrame frame ) { staticDefaultInterruptRoutine( 254 );  }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int255( InterruptFrame frame ) { staticDefaultInterruptRoutine( 255 );  }

static void InitializeBaseVectorCallback( GenericGlyphStringOutputter outputter )
{
    defaultGlyphStringOutputter = outputter;

    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[0], baseInterruptVectorRoutine_Int0, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[1], baseInterruptVectorRoutine_Int1, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[2], baseInterruptVectorRoutine_Int2, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[3], baseInterruptVectorRoutine_Int3, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[4], baseInterruptVectorRoutine_Int4, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[5], baseInterruptVectorRoutine_Int5, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[6], baseInterruptVectorRoutine_Int6, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[7], baseInterruptVectorRoutine_Int7, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[8], baseInterruptVectorRoutine_Int8, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[9], baseInterruptVectorRoutine_Int9, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[10], baseInterruptVectorRoutine_Int10, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[11], baseInterruptVectorRoutine_Int11, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[12], baseInterruptVectorRoutine_Int12, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[13], baseInterruptVectorRoutine_Int13, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[14], baseInterruptVectorRoutine_Int14, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[15], baseInterruptVectorRoutine_Int15, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[16], baseInterruptVectorRoutine_Int16, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[17], baseInterruptVectorRoutine_Int17, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[18], baseInterruptVectorRoutine_Int18, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[19], baseInterruptVectorRoutine_Int19, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[20], baseInterruptVectorRoutine_Int20, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[21], baseInterruptVectorRoutine_Int21, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[22], baseInterruptVectorRoutine_Int22, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[23], baseInterruptVectorRoutine_Int23, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[24], baseInterruptVectorRoutine_Int24, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[25], baseInterruptVectorRoutine_Int25, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[26], baseInterruptVectorRoutine_Int26, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[27], baseInterruptVectorRoutine_Int27, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[28], baseInterruptVectorRoutine_Int28, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[29], baseInterruptVectorRoutine_Int29, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[30], baseInterruptVectorRoutine_Int30, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[31], baseInterruptVectorRoutine_Int31, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[32], baseInterruptVectorRoutine_Int32, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[33], baseInterruptVectorRoutine_Int33, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[34], baseInterruptVectorRoutine_Int34, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[35], baseInterruptVectorRoutine_Int35, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[36], baseInterruptVectorRoutine_Int36, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[37], baseInterruptVectorRoutine_Int37, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[38], baseInterruptVectorRoutine_Int38, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[39], baseInterruptVectorRoutine_Int39, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[40], baseInterruptVectorRoutine_Int40, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[41], baseInterruptVectorRoutine_Int41, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[42], baseInterruptVectorRoutine_Int42, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[43], baseInterruptVectorRoutine_Int43, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[44], baseInterruptVectorRoutine_Int44, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[45], baseInterruptVectorRoutine_Int45, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[46], baseInterruptVectorRoutine_Int46, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[47], baseInterruptVectorRoutine_Int47, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[48], baseInterruptVectorRoutine_Int48, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[49], baseInterruptVectorRoutine_Int49, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[50], baseInterruptVectorRoutine_Int50, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[51], baseInterruptVectorRoutine_Int51, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[52], baseInterruptVectorRoutine_Int52, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[53], baseInterruptVectorRoutine_Int53, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[54], baseInterruptVectorRoutine_Int54, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[55], baseInterruptVectorRoutine_Int55, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[56], baseInterruptVectorRoutine_Int56, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[57], baseInterruptVectorRoutine_Int57, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[58], baseInterruptVectorRoutine_Int58, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[59], baseInterruptVectorRoutine_Int59, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[60], baseInterruptVectorRoutine_Int60, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[61], baseInterruptVectorRoutine_Int61, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[62], baseInterruptVectorRoutine_Int62, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[63], baseInterruptVectorRoutine_Int63, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[64], baseInterruptVectorRoutine_Int64, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[65], baseInterruptVectorRoutine_Int65, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[66], baseInterruptVectorRoutine_Int66, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[67], baseInterruptVectorRoutine_Int67, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[68], baseInterruptVectorRoutine_Int68, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[69], baseInterruptVectorRoutine_Int69, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[70], baseInterruptVectorRoutine_Int70, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[71], baseInterruptVectorRoutine_Int71, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[72], baseInterruptVectorRoutine_Int72, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[73], baseInterruptVectorRoutine_Int73, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[74], baseInterruptVectorRoutine_Int74, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[75], baseInterruptVectorRoutine_Int75, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[76], baseInterruptVectorRoutine_Int76, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[77], baseInterruptVectorRoutine_Int77, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[78], baseInterruptVectorRoutine_Int78, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[79], baseInterruptVectorRoutine_Int79, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[80], baseInterruptVectorRoutine_Int80, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[81], baseInterruptVectorRoutine_Int81, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[82], baseInterruptVectorRoutine_Int82, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[83], baseInterruptVectorRoutine_Int83, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[84], baseInterruptVectorRoutine_Int84, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[85], baseInterruptVectorRoutine_Int85, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[86], baseInterruptVectorRoutine_Int86, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[87], baseInterruptVectorRoutine_Int87, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[88], baseInterruptVectorRoutine_Int88, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[89], baseInterruptVectorRoutine_Int89, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[90], baseInterruptVectorRoutine_Int90, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[91], baseInterruptVectorRoutine_Int91, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[92], baseInterruptVectorRoutine_Int92, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[93], baseInterruptVectorRoutine_Int93, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[94], baseInterruptVectorRoutine_Int94, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[95], baseInterruptVectorRoutine_Int95, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[96], baseInterruptVectorRoutine_Int96, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[97], baseInterruptVectorRoutine_Int97, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[98], baseInterruptVectorRoutine_Int98, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[99], baseInterruptVectorRoutine_Int99, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[100], baseInterruptVectorRoutine_Int100, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[101], baseInterruptVectorRoutine_Int101, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[102], baseInterruptVectorRoutine_Int102, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[103], baseInterruptVectorRoutine_Int103, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[104], baseInterruptVectorRoutine_Int104, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[105], baseInterruptVectorRoutine_Int105, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[106], baseInterruptVectorRoutine_Int106, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[107], baseInterruptVectorRoutine_Int107, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[108], baseInterruptVectorRoutine_Int108, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[109], baseInterruptVectorRoutine_Int109, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[110], baseInterruptVectorRoutine_Int110, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[111], baseInterruptVectorRoutine_Int111, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[112], baseInterruptVectorRoutine_Int112, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[113], baseInterruptVectorRoutine_Int113, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[114], baseInterruptVectorRoutine_Int114, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[115], baseInterruptVectorRoutine_Int115, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[116], baseInterruptVectorRoutine_Int116, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[117], baseInterruptVectorRoutine_Int117, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[118], baseInterruptVectorRoutine_Int118, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[119], baseInterruptVectorRoutine_Int119, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[120], baseInterruptVectorRoutine_Int120, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[121], baseInterruptVectorRoutine_Int121, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[122], baseInterruptVectorRoutine_Int122, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[123], baseInterruptVectorRoutine_Int123, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[124], baseInterruptVectorRoutine_Int124, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[125], baseInterruptVectorRoutine_Int125, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[126], baseInterruptVectorRoutine_Int126, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[127], baseInterruptVectorRoutine_Int127, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[128], baseInterruptVectorRoutine_Int128, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[129], baseInterruptVectorRoutine_Int129, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[130], baseInterruptVectorRoutine_Int130, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[131], baseInterruptVectorRoutine_Int131, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[132], baseInterruptVectorRoutine_Int132, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[133], baseInterruptVectorRoutine_Int133, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[134], baseInterruptVectorRoutine_Int134, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[135], baseInterruptVectorRoutine_Int135, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[136], baseInterruptVectorRoutine_Int136, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[137], baseInterruptVectorRoutine_Int137, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[138], baseInterruptVectorRoutine_Int138, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[139], baseInterruptVectorRoutine_Int139, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[140], baseInterruptVectorRoutine_Int140, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[141], baseInterruptVectorRoutine_Int141, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[142], baseInterruptVectorRoutine_Int142, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[143], baseInterruptVectorRoutine_Int143, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[144], baseInterruptVectorRoutine_Int144, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[145], baseInterruptVectorRoutine_Int145, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[146], baseInterruptVectorRoutine_Int146, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[147], baseInterruptVectorRoutine_Int147, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[148], baseInterruptVectorRoutine_Int148, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[149], baseInterruptVectorRoutine_Int149, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[150], baseInterruptVectorRoutine_Int150, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[151], baseInterruptVectorRoutine_Int151, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[152], baseInterruptVectorRoutine_Int152, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[153], baseInterruptVectorRoutine_Int153, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[154], baseInterruptVectorRoutine_Int154, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[155], baseInterruptVectorRoutine_Int155, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[156], baseInterruptVectorRoutine_Int156, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[157], baseInterruptVectorRoutine_Int157, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[158], baseInterruptVectorRoutine_Int158, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[159], baseInterruptVectorRoutine_Int159, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[160], baseInterruptVectorRoutine_Int160, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[161], baseInterruptVectorRoutine_Int161, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[162], baseInterruptVectorRoutine_Int162, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[163], baseInterruptVectorRoutine_Int163, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[164], baseInterruptVectorRoutine_Int164, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[165], baseInterruptVectorRoutine_Int165, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[166], baseInterruptVectorRoutine_Int166, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[167], baseInterruptVectorRoutine_Int167, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[168], baseInterruptVectorRoutine_Int168, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[169], baseInterruptVectorRoutine_Int169, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[170], baseInterruptVectorRoutine_Int170, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[171], baseInterruptVectorRoutine_Int171, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[172], baseInterruptVectorRoutine_Int172, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[173], baseInterruptVectorRoutine_Int173, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[174], baseInterruptVectorRoutine_Int174, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[175], baseInterruptVectorRoutine_Int175, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[176], baseInterruptVectorRoutine_Int176, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[177], baseInterruptVectorRoutine_Int177, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[178], baseInterruptVectorRoutine_Int178, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[179], baseInterruptVectorRoutine_Int179, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[180], baseInterruptVectorRoutine_Int180, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[181], baseInterruptVectorRoutine_Int181, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[182], baseInterruptVectorRoutine_Int182, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[183], baseInterruptVectorRoutine_Int183, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[184], baseInterruptVectorRoutine_Int184, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[185], baseInterruptVectorRoutine_Int185, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[186], baseInterruptVectorRoutine_Int186, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[187], baseInterruptVectorRoutine_Int187, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[188], baseInterruptVectorRoutine_Int188, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[189], baseInterruptVectorRoutine_Int189, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[190], baseInterruptVectorRoutine_Int190, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[191], baseInterruptVectorRoutine_Int191, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[192], baseInterruptVectorRoutine_Int192, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[193], baseInterruptVectorRoutine_Int193, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[194], baseInterruptVectorRoutine_Int194, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[195], baseInterruptVectorRoutine_Int195, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[196], baseInterruptVectorRoutine_Int196, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[197], baseInterruptVectorRoutine_Int197, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[198], baseInterruptVectorRoutine_Int198, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[199], baseInterruptVectorRoutine_Int199, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[200], baseInterruptVectorRoutine_Int200, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[201], baseInterruptVectorRoutine_Int201, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[202], baseInterruptVectorRoutine_Int202, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[203], baseInterruptVectorRoutine_Int203, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[204], baseInterruptVectorRoutine_Int204, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[205], baseInterruptVectorRoutine_Int205, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[206], baseInterruptVectorRoutine_Int206, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[207], baseInterruptVectorRoutine_Int207, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[208], baseInterruptVectorRoutine_Int208, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[209], baseInterruptVectorRoutine_Int209, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[210], baseInterruptVectorRoutine_Int210, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[211], baseInterruptVectorRoutine_Int211, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[212], baseInterruptVectorRoutine_Int212, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[213], baseInterruptVectorRoutine_Int213, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[214], baseInterruptVectorRoutine_Int214, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[215], baseInterruptVectorRoutine_Int215, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[216], baseInterruptVectorRoutine_Int216, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[217], baseInterruptVectorRoutine_Int217, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[218], baseInterruptVectorRoutine_Int218, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[219], baseInterruptVectorRoutine_Int219, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[220], baseInterruptVectorRoutine_Int220, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[221], baseInterruptVectorRoutine_Int221, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[222], baseInterruptVectorRoutine_Int222, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[223], baseInterruptVectorRoutine_Int223, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[224], baseInterruptVectorRoutine_Int224, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[225], baseInterruptVectorRoutine_Int225, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[226], baseInterruptVectorRoutine_Int226, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[227], baseInterruptVectorRoutine_Int227, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[228], baseInterruptVectorRoutine_Int228, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[229], baseInterruptVectorRoutine_Int229, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[230], baseInterruptVectorRoutine_Int230, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[231], baseInterruptVectorRoutine_Int231, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[232], baseInterruptVectorRoutine_Int232, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[233], baseInterruptVectorRoutine_Int233, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[234], baseInterruptVectorRoutine_Int234, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[235], baseInterruptVectorRoutine_Int235, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[236], baseInterruptVectorRoutine_Int236, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[237], baseInterruptVectorRoutine_Int237, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[238], baseInterruptVectorRoutine_Int238, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[239], baseInterruptVectorRoutine_Int239, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[240], baseInterruptVectorRoutine_Int240, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[241], baseInterruptVectorRoutine_Int241, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[242], baseInterruptVectorRoutine_Int242, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[243], baseInterruptVectorRoutine_Int243, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[244], baseInterruptVectorRoutine_Int244, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[245], baseInterruptVectorRoutine_Int245, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[246], baseInterruptVectorRoutine_Int246, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[247], baseInterruptVectorRoutine_Int247, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[248], baseInterruptVectorRoutine_Int248, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[249], baseInterruptVectorRoutine_Int249, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[250], baseInterruptVectorRoutine_Int250, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[251], baseInterruptVectorRoutine_Int251, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[252], baseInterruptVectorRoutine_Int252, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[253], baseInterruptVectorRoutine_Int253, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[254], baseInterruptVectorRoutine_Int254, CPL0, 0 );
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[255], baseInterruptVectorRoutine_Int255, CPL0, 0 );
}

Error PopulateInterruptDescriptorTableBuilder(InterruptDescriptorTableBuilder b) {
    b->InitializeBaseVectorCallback = InitializeBaseVectorCallback;
    b->ActivateTable = ActivateTable;
    b->SetInterruptVectorCallback = SetInterruptVectorCallback;
    return NoError;
}

static void Reinitalize( uint8_t remappedInterruptNumberForIRQ0, uint8_t remappedInterruptNumberForIRQ8 )
{
    uint8_t pic1Mask, pic2Mask;

    pic1Mask = ReadByteFromIOPort( PIC1_DATA_PORT );
    pic2Mask = ReadByteFromIOPort( PIC2_DATA_PORT );

    WriteByteToIOPort( PIC1_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_IC4 );
    WriteByteToIOPort( PIC2_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_IC4 );

    WriteByteToIOPort( PIC1_DATA_PORT, remappedInterruptNumberForIRQ0 );
    WriteByteToIOPort( PIC2_DATA_PORT, remappedInterruptNumberForIRQ8 );

    WriteByteToIOPort( PIC1_DATA_PORT, PIC_ICW3_ID0_IR2 ); // PIC1->PIC2 cascading IRQ is now 2
    WriteByteToIOPort( PIC2_DATA_PORT, 2 );

    WriteByteToIOPort( PIC1_DATA_PORT, PIC_ICW4_IS_8086 );
    WriteByteToIOPort( PIC2_DATA_PORT, PIC_ICW4_IS_8086 );

    WriteByteToIOPort( PIC1_DATA_PORT, pic1Mask );
    WriteByteToIOPort( PIC2_DATA_PORT, pic2Mask );
}

static void Disable()
{
    WriteByteToIOPort( PIC2_DATA_PORT, 0xff );
    WriteByteToIOPort( PIC1_DATA_PORT, 0xff );
}

Error PopulatePICsConfigurator( PICsConfigurator p )
{
    p->Reinitalize = Reinitalize;
    p->Disable = Disable;
}