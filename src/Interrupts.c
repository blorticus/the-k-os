#include <Interrupts.h>

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

static void baseInterruptVectorRoutine_Int0() { staticDefaultTableInterruptVectorCallbacks[0]( 0 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int1() { staticDefaultTableInterruptVectorCallbacks[1]( 1 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int2() { staticDefaultTableInterruptVectorCallbacks[2]( 2 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int3() { staticDefaultTableInterruptVectorCallbacks[3]( 3 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int4() { staticDefaultTableInterruptVectorCallbacks[4]( 4 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int5() { staticDefaultTableInterruptVectorCallbacks[5]( 5 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int6() { staticDefaultTableInterruptVectorCallbacks[6]( 6 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int7() { staticDefaultTableInterruptVectorCallbacks[7]( 7 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int8() { staticDefaultTableInterruptVectorCallbacks[8]( 8 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int9() { staticDefaultTableInterruptVectorCallbacks[9]( 9 ); asm volatile( "iretq" ); }   
static void baseInterruptVectorRoutine_Int10() { staticDefaultTableInterruptVectorCallbacks[10]( 10 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int11() { staticDefaultTableInterruptVectorCallbacks[11]( 11 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int12() { staticDefaultTableInterruptVectorCallbacks[12]( 12 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int13() { staticDefaultTableInterruptVectorCallbacks[13]( 13 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int14() { staticDefaultTableInterruptVectorCallbacks[14]( 14 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int15() { staticDefaultTableInterruptVectorCallbacks[15]( 15 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int16() { staticDefaultTableInterruptVectorCallbacks[16]( 16 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int17() { staticDefaultTableInterruptVectorCallbacks[17]( 17 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int18() { staticDefaultTableInterruptVectorCallbacks[18]( 18 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int19() { staticDefaultTableInterruptVectorCallbacks[19]( 19 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int20() { staticDefaultTableInterruptVectorCallbacks[20]( 20 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int21() { staticDefaultTableInterruptVectorCallbacks[21]( 21 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int22() { staticDefaultTableInterruptVectorCallbacks[22]( 22 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int23() { staticDefaultTableInterruptVectorCallbacks[23]( 23 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int24() { staticDefaultTableInterruptVectorCallbacks[24]( 24 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int25() { staticDefaultTableInterruptVectorCallbacks[25]( 25 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int26() { staticDefaultTableInterruptVectorCallbacks[26]( 26 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int27() { staticDefaultTableInterruptVectorCallbacks[27]( 27 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int28() { staticDefaultTableInterruptVectorCallbacks[28]( 28 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int29() { staticDefaultTableInterruptVectorCallbacks[29]( 29 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int30() { staticDefaultTableInterruptVectorCallbacks[30]( 30 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int31() { staticDefaultTableInterruptVectorCallbacks[31]( 31 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int32() { staticDefaultTableInterruptVectorCallbacks[32]( 32 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int33() { staticDefaultTableInterruptVectorCallbacks[33]( 33 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int34() { staticDefaultTableInterruptVectorCallbacks[34]( 34 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int35() { staticDefaultTableInterruptVectorCallbacks[35]( 35 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int36() { staticDefaultTableInterruptVectorCallbacks[36]( 36 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int37() { staticDefaultTableInterruptVectorCallbacks[37]( 37 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int38() { staticDefaultTableInterruptVectorCallbacks[38]( 38 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int39() { staticDefaultTableInterruptVectorCallbacks[39]( 39 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int40() { staticDefaultTableInterruptVectorCallbacks[40]( 40 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int41() { staticDefaultTableInterruptVectorCallbacks[41]( 41 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int42() { staticDefaultTableInterruptVectorCallbacks[42]( 42 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int43() { staticDefaultTableInterruptVectorCallbacks[43]( 43 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int44() { staticDefaultTableInterruptVectorCallbacks[44]( 44 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int45() { staticDefaultTableInterruptVectorCallbacks[45]( 45 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int46() { staticDefaultTableInterruptVectorCallbacks[46]( 46 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int47() { staticDefaultTableInterruptVectorCallbacks[47]( 47 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int48() { staticDefaultTableInterruptVectorCallbacks[48]( 48 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int49() { staticDefaultTableInterruptVectorCallbacks[49]( 49 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int50() { staticDefaultTableInterruptVectorCallbacks[50]( 50 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int51() { staticDefaultTableInterruptVectorCallbacks[51]( 51 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int52() { staticDefaultTableInterruptVectorCallbacks[52]( 52 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int53() { staticDefaultTableInterruptVectorCallbacks[53]( 53 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int54() { staticDefaultTableInterruptVectorCallbacks[54]( 54 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int55() { staticDefaultTableInterruptVectorCallbacks[55]( 55 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int56() { staticDefaultTableInterruptVectorCallbacks[56]( 56 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int57() { staticDefaultTableInterruptVectorCallbacks[57]( 57 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int58() { staticDefaultTableInterruptVectorCallbacks[58]( 58 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int59() { staticDefaultTableInterruptVectorCallbacks[59]( 59 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int60() { staticDefaultTableInterruptVectorCallbacks[60]( 60 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int61() { staticDefaultTableInterruptVectorCallbacks[61]( 61 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int62() { staticDefaultTableInterruptVectorCallbacks[62]( 62 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int63() { staticDefaultTableInterruptVectorCallbacks[63]( 63 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int64() { staticDefaultTableInterruptVectorCallbacks[64]( 64 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int65() { staticDefaultTableInterruptVectorCallbacks[65]( 65 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int66() { staticDefaultTableInterruptVectorCallbacks[66]( 66 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int67() { staticDefaultTableInterruptVectorCallbacks[67]( 67 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int68() { staticDefaultTableInterruptVectorCallbacks[68]( 68 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int69() { staticDefaultTableInterruptVectorCallbacks[69]( 69 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int70() { staticDefaultTableInterruptVectorCallbacks[70]( 70 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int71() { staticDefaultTableInterruptVectorCallbacks[71]( 71 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int72() { staticDefaultTableInterruptVectorCallbacks[72]( 72 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int73() { staticDefaultTableInterruptVectorCallbacks[73]( 73 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int74() { staticDefaultTableInterruptVectorCallbacks[74]( 74 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int75() { staticDefaultTableInterruptVectorCallbacks[75]( 75 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int76() { staticDefaultTableInterruptVectorCallbacks[76]( 76 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int77() { staticDefaultTableInterruptVectorCallbacks[77]( 77 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int78() { staticDefaultTableInterruptVectorCallbacks[78]( 78 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int79() { staticDefaultTableInterruptVectorCallbacks[79]( 79 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int80() { staticDefaultTableInterruptVectorCallbacks[80]( 80 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int81() { staticDefaultTableInterruptVectorCallbacks[81]( 81 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int82() { staticDefaultTableInterruptVectorCallbacks[82]( 82 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int83() { staticDefaultTableInterruptVectorCallbacks[83]( 83 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int84() { staticDefaultTableInterruptVectorCallbacks[84]( 84 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int85() { staticDefaultTableInterruptVectorCallbacks[85]( 85 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int86() { staticDefaultTableInterruptVectorCallbacks[86]( 86 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int87() { staticDefaultTableInterruptVectorCallbacks[87]( 87 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int88() { staticDefaultTableInterruptVectorCallbacks[88]( 88 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int89() { staticDefaultTableInterruptVectorCallbacks[89]( 89 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int90() { staticDefaultTableInterruptVectorCallbacks[90]( 90 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int91() { staticDefaultTableInterruptVectorCallbacks[91]( 91 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int92() { staticDefaultTableInterruptVectorCallbacks[92]( 92 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int93() { staticDefaultTableInterruptVectorCallbacks[93]( 93 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int94() { staticDefaultTableInterruptVectorCallbacks[94]( 94 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int95() { staticDefaultTableInterruptVectorCallbacks[95]( 95 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int96() { staticDefaultTableInterruptVectorCallbacks[96]( 96 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int97() { staticDefaultTableInterruptVectorCallbacks[97]( 97 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int98() { staticDefaultTableInterruptVectorCallbacks[98]( 98 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int99() { staticDefaultTableInterruptVectorCallbacks[99]( 99 ); asm volatile( "iretq" ); }
static __attribute__ ((interrupt)) void baseInterruptVectorRoutine_Int100( InterruptFrame frame ) { staticDefaultTableInterruptVectorCallbacks[100]( 100 ); }
static void baseInterruptVectorRoutine_Int101() { staticDefaultTableInterruptVectorCallbacks[101]( 101 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int102() { staticDefaultTableInterruptVectorCallbacks[102]( 102 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int103() { staticDefaultTableInterruptVectorCallbacks[103]( 103 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int104() { staticDefaultTableInterruptVectorCallbacks[104]( 104 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int105() { staticDefaultTableInterruptVectorCallbacks[105]( 105 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int106() { staticDefaultTableInterruptVectorCallbacks[106]( 106 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int107() { staticDefaultTableInterruptVectorCallbacks[107]( 107 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int108() { staticDefaultTableInterruptVectorCallbacks[108]( 108 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int109() { staticDefaultTableInterruptVectorCallbacks[109]( 109 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int110() { staticDefaultTableInterruptVectorCallbacks[110]( 110 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int111() { staticDefaultTableInterruptVectorCallbacks[111]( 111 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int112() { staticDefaultTableInterruptVectorCallbacks[112]( 112 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int113() { staticDefaultTableInterruptVectorCallbacks[113]( 113 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int114() { staticDefaultTableInterruptVectorCallbacks[114]( 114 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int115() { staticDefaultTableInterruptVectorCallbacks[115]( 115 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int116() { staticDefaultTableInterruptVectorCallbacks[116]( 116 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int117() { staticDefaultTableInterruptVectorCallbacks[117]( 117 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int118() { staticDefaultTableInterruptVectorCallbacks[118]( 118 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int119() { staticDefaultTableInterruptVectorCallbacks[119]( 119 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int120() { staticDefaultTableInterruptVectorCallbacks[120]( 120 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int121() { staticDefaultTableInterruptVectorCallbacks[121]( 121 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int122() { staticDefaultTableInterruptVectorCallbacks[122]( 122 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int123() { staticDefaultTableInterruptVectorCallbacks[123]( 123 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int124() { staticDefaultTableInterruptVectorCallbacks[124]( 124 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int125() { staticDefaultTableInterruptVectorCallbacks[125]( 125 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int126() { staticDefaultTableInterruptVectorCallbacks[126]( 126 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int127() { staticDefaultTableInterruptVectorCallbacks[127]( 127 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int128() { staticDefaultTableInterruptVectorCallbacks[128]( 128 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int129() { staticDefaultTableInterruptVectorCallbacks[129]( 129 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int130() { staticDefaultTableInterruptVectorCallbacks[130]( 130 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int131() { staticDefaultTableInterruptVectorCallbacks[131]( 131 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int132() { staticDefaultTableInterruptVectorCallbacks[132]( 132 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int133() { staticDefaultTableInterruptVectorCallbacks[133]( 133 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int134() { staticDefaultTableInterruptVectorCallbacks[134]( 134 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int135() { staticDefaultTableInterruptVectorCallbacks[135]( 135 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int136() { staticDefaultTableInterruptVectorCallbacks[136]( 136 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int137() { staticDefaultTableInterruptVectorCallbacks[137]( 137 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int138() { staticDefaultTableInterruptVectorCallbacks[138]( 138 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int139() { staticDefaultTableInterruptVectorCallbacks[139]( 139 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int140() { staticDefaultTableInterruptVectorCallbacks[140]( 140 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int141() { staticDefaultTableInterruptVectorCallbacks[141]( 141 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int142() { staticDefaultTableInterruptVectorCallbacks[142]( 142 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int143() { staticDefaultTableInterruptVectorCallbacks[143]( 143 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int144() { staticDefaultTableInterruptVectorCallbacks[144]( 144 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int145() { staticDefaultTableInterruptVectorCallbacks[145]( 145 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int146() { staticDefaultTableInterruptVectorCallbacks[146]( 146 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int147() { staticDefaultTableInterruptVectorCallbacks[147]( 147 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int148() { staticDefaultTableInterruptVectorCallbacks[148]( 148 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int149() { staticDefaultTableInterruptVectorCallbacks[149]( 149 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int150() { staticDefaultTableInterruptVectorCallbacks[150]( 150 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int151() { staticDefaultTableInterruptVectorCallbacks[151]( 151 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int152() { staticDefaultTableInterruptVectorCallbacks[152]( 152 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int153() { staticDefaultTableInterruptVectorCallbacks[153]( 153 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int154() { staticDefaultTableInterruptVectorCallbacks[154]( 154 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int155() { staticDefaultTableInterruptVectorCallbacks[155]( 155 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int156() { staticDefaultTableInterruptVectorCallbacks[156]( 156 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int157() { staticDefaultTableInterruptVectorCallbacks[157]( 157 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int158() { staticDefaultTableInterruptVectorCallbacks[158]( 158 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int159() { staticDefaultTableInterruptVectorCallbacks[159]( 159 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int160() { staticDefaultTableInterruptVectorCallbacks[160]( 160 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int161() { staticDefaultTableInterruptVectorCallbacks[161]( 161 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int162() { staticDefaultTableInterruptVectorCallbacks[162]( 162 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int163() { staticDefaultTableInterruptVectorCallbacks[163]( 163 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int164() { staticDefaultTableInterruptVectorCallbacks[164]( 164 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int165() { staticDefaultTableInterruptVectorCallbacks[165]( 165 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int166() { staticDefaultTableInterruptVectorCallbacks[166]( 166 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int167() { staticDefaultTableInterruptVectorCallbacks[167]( 167 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int168() { staticDefaultTableInterruptVectorCallbacks[168]( 168 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int169() { staticDefaultTableInterruptVectorCallbacks[169]( 169 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int170() { staticDefaultTableInterruptVectorCallbacks[170]( 170 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int171() { staticDefaultTableInterruptVectorCallbacks[171]( 171 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int172() { staticDefaultTableInterruptVectorCallbacks[172]( 172 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int173() { staticDefaultTableInterruptVectorCallbacks[173]( 173 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int174() { staticDefaultTableInterruptVectorCallbacks[174]( 174 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int175() { staticDefaultTableInterruptVectorCallbacks[175]( 175 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int176() { staticDefaultTableInterruptVectorCallbacks[176]( 176 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int177() { staticDefaultTableInterruptVectorCallbacks[177]( 177 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int178() { staticDefaultTableInterruptVectorCallbacks[178]( 178 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int179() { staticDefaultTableInterruptVectorCallbacks[179]( 179 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int180() { staticDefaultTableInterruptVectorCallbacks[180]( 180 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int181() { staticDefaultTableInterruptVectorCallbacks[181]( 181 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int182() { staticDefaultTableInterruptVectorCallbacks[182]( 182 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int183() { staticDefaultTableInterruptVectorCallbacks[183]( 183 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int184() { staticDefaultTableInterruptVectorCallbacks[184]( 184 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int185() { staticDefaultTableInterruptVectorCallbacks[185]( 185 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int186() { staticDefaultTableInterruptVectorCallbacks[186]( 186 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int187() { staticDefaultTableInterruptVectorCallbacks[187]( 187 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int188() { staticDefaultTableInterruptVectorCallbacks[188]( 188 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int189() { staticDefaultTableInterruptVectorCallbacks[189]( 189 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int190() { staticDefaultTableInterruptVectorCallbacks[190]( 190 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int191() { staticDefaultTableInterruptVectorCallbacks[191]( 191 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int192() { staticDefaultTableInterruptVectorCallbacks[192]( 192 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int193() { staticDefaultTableInterruptVectorCallbacks[193]( 193 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int194() { staticDefaultTableInterruptVectorCallbacks[194]( 194 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int195() { staticDefaultTableInterruptVectorCallbacks[195]( 195 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int196() { staticDefaultTableInterruptVectorCallbacks[196]( 196 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int197() { staticDefaultTableInterruptVectorCallbacks[197]( 197 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int198() { staticDefaultTableInterruptVectorCallbacks[198]( 198 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int199() { staticDefaultTableInterruptVectorCallbacks[199]( 199 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int200() { staticDefaultTableInterruptVectorCallbacks[200]( 200 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int201() { staticDefaultTableInterruptVectorCallbacks[201]( 201 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int202() { staticDefaultTableInterruptVectorCallbacks[202]( 202 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int203() { staticDefaultTableInterruptVectorCallbacks[203]( 203 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int204() { staticDefaultTableInterruptVectorCallbacks[204]( 204 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int205() { staticDefaultTableInterruptVectorCallbacks[205]( 205 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int206() { staticDefaultTableInterruptVectorCallbacks[206]( 206 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int207() { staticDefaultTableInterruptVectorCallbacks[207]( 207 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int208() { staticDefaultTableInterruptVectorCallbacks[208]( 208 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int209() { staticDefaultTableInterruptVectorCallbacks[209]( 209 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int210() { staticDefaultTableInterruptVectorCallbacks[210]( 210 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int211() { staticDefaultTableInterruptVectorCallbacks[211]( 211 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int212() { staticDefaultTableInterruptVectorCallbacks[212]( 212 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int213() { staticDefaultTableInterruptVectorCallbacks[213]( 213 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int214() { staticDefaultTableInterruptVectorCallbacks[214]( 214 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int215() { staticDefaultTableInterruptVectorCallbacks[215]( 215 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int216() { staticDefaultTableInterruptVectorCallbacks[216]( 216 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int217() { staticDefaultTableInterruptVectorCallbacks[217]( 217 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int218() { staticDefaultTableInterruptVectorCallbacks[218]( 218 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int219() { staticDefaultTableInterruptVectorCallbacks[219]( 219 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int220() { staticDefaultTableInterruptVectorCallbacks[220]( 220 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int221() { staticDefaultTableInterruptVectorCallbacks[221]( 221 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int222() { staticDefaultTableInterruptVectorCallbacks[222]( 222 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int223() { staticDefaultTableInterruptVectorCallbacks[223]( 223 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int224() { staticDefaultTableInterruptVectorCallbacks[224]( 224 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int225() { staticDefaultTableInterruptVectorCallbacks[225]( 225 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int226() { staticDefaultTableInterruptVectorCallbacks[226]( 226 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int227() { staticDefaultTableInterruptVectorCallbacks[227]( 227 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int228() { staticDefaultTableInterruptVectorCallbacks[228]( 228 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int229() { staticDefaultTableInterruptVectorCallbacks[229]( 229 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int230() { staticDefaultTableInterruptVectorCallbacks[230]( 230 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int231() { staticDefaultTableInterruptVectorCallbacks[231]( 231 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int232() { staticDefaultTableInterruptVectorCallbacks[232]( 232 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int233() { staticDefaultTableInterruptVectorCallbacks[233]( 233 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int234() { staticDefaultTableInterruptVectorCallbacks[234]( 234 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int235() { staticDefaultTableInterruptVectorCallbacks[235]( 235 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int236() { staticDefaultTableInterruptVectorCallbacks[236]( 236 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int237() { staticDefaultTableInterruptVectorCallbacks[237]( 237 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int238() { staticDefaultTableInterruptVectorCallbacks[238]( 238 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int239() { staticDefaultTableInterruptVectorCallbacks[239]( 239 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int240() { staticDefaultTableInterruptVectorCallbacks[240]( 240 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int241() { staticDefaultTableInterruptVectorCallbacks[241]( 241 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int242() { staticDefaultTableInterruptVectorCallbacks[242]( 242 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int243() { staticDefaultTableInterruptVectorCallbacks[243]( 243 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int244() { staticDefaultTableInterruptVectorCallbacks[244]( 244 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int245() { staticDefaultTableInterruptVectorCallbacks[245]( 245 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int246() { staticDefaultTableInterruptVectorCallbacks[246]( 246 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int247() { staticDefaultTableInterruptVectorCallbacks[247]( 247 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int248() { staticDefaultTableInterruptVectorCallbacks[248]( 248 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int249() { staticDefaultTableInterruptVectorCallbacks[249]( 249 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int250() { staticDefaultTableInterruptVectorCallbacks[250]( 250 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int251() { staticDefaultTableInterruptVectorCallbacks[251]( 251 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int252() { staticDefaultTableInterruptVectorCallbacks[252]( 252 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int253() { staticDefaultTableInterruptVectorCallbacks[253]( 253 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int254() { staticDefaultTableInterruptVectorCallbacks[254]( 254 ); asm volatile( "iretq" ); }
static void baseInterruptVectorRoutine_Int255() { staticDefaultTableInterruptVectorCallbacks[255]( 255 ); asm volatile( "iretq" ); }

static void InitializeBaseVectorCallback()
{
    PopulateInterruptDescriptor( &staticDefaultInterruptDescriptors[0], baseInterruptVectorRoutine_Int0, 0, 0 );
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
