#include <TestSuite.h>
#include <Cpu.h>
#include <stdlib.h>

typedef struct testCpuLeafRetval_t {
    unsigned int leaf;
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
} testCpuLeafRetval;

typedef struct testCpuLeafRetvalMap_t {
    testCpuLeafRetval* map;
    unsigned int numberOfMapEntries;
} testCpuLeafRetvalMap;

unsigned int nextCpuidMaxReturnValue = 0;
unsigned int* nextCpuidMaxSig = 0;
testCpuLeafRetvalMap* cpuidRetvalMap = 0;

unsigned int getCpuidMax( __attribute__((unused)) unsigned int ext, unsigned int *sig)
{
    if (sig != 0)
        sig = nextCpuidMaxSig;

    return nextCpuidMaxReturnValue;
}

unsigned int getCpuid(unsigned int leaf, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
    if (!cpuidRetvalMap)
        return 0;

    testCpuLeafRetval *map = cpuidRetvalMap->map;
    for (unsigned int i = 0; i < cpuidRetvalMap->numberOfMapEntries; i++)
    {
        if (map[i].leaf == leaf)
        {
            *eax = map[i].eax;
            *ebx = map[i].ebx;
            *ecx = map[i].ecx;
            *edx = map[i].edx;

            return nextCpuidMaxReturnValue;
        }
    }

    return 0;
}

int main( void )
{
    TestSuite suite = CreateTestSuite( "Cpu" );

    nextCpuidMaxReturnValue = 0xd;
    nextCpuidMaxSig = (unsigned int[]){ 'T', 'e', 's', 't' };
    cpuidRetvalMap = calloc( 1, sizeof( struct testCpuLeafRetval_t) );
    cpuidRetvalMap->map = (testCpuLeafRetval[]){
        {.leaf = 0, .eax = 0xd, .ebx = 0x74736554, .edx = 0x69755320, .ecx = 0x20206574}, // "Test Suite  "
        {.leaf = 1, .eax = 0x000001110, .ebx = 0x01040801, .edx = 0x08ffdfff, .ecx = 0x0e56f049 }};
    cpuidRetvalMap->numberOfMapEntries = 2;

    CpuInformation c = calloc( 1, sizeof(CpuInformation_t) );
    Error e = PopulateCpuInformation( c );

    suite->AssertEquals->Int32( suite, NoError, e, "PopulateCpuInformation() return result" );
    suite->AssertEquals->RuneString( suite, U"Test Suite  ", c->CpuidManufacturerIdString, 12, "CpuidManufacturerIdString" );

    suite->AssertIs->True(suite, c->CpuSupports(c, OnboardX87FPU), "Supports OnboardX87FPU");
    suite->AssertIs->False(suite, c->CpuSupports(c, IA64ProcessorEmulatingX86), "Supports IA64ProcessorEmulatingX86");

    return suite->Done( suite );
}