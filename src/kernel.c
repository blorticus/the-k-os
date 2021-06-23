#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>
#include <FrameBuffer.h>
#include <Error.h>
#include <TextTerminal.h>
#include <Interrupts.h>
#include <String.h>
#include <Cpu.h>
#include <stdarg.h>

static uint8_t stack[16384];

static Stivale2Tag_t RsdpRequestTag = {
    .identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
    .next = (uintptr_t)0
};

static Stivale2HeaderTagFramebuffer_t FramebufferRequestTag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&RsdpRequestTag,
    },

    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static Stivale2Header_t stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 0,
    .tags = (uintptr_t)&FramebufferRequestTag,
};

static struct FrameBuffer_t _fb;
static FrameBuffer fb = &_fb;
static struct TextTerminal_t _term;
static TextTerminal term = &_term;

static InterruptDescriptorTableBuilder_t _idtBuilder;
static InterruptDescriptorTableBuilder idtBuilder = &_idtBuilder;

static CpuInformation_t _cpuInfo;
static CpuInformation cpuInfo = &_cpuInfo;

static PICsConfigurator_t _picsConfigurator;
static PICsConfigurator picsConfigurator = &_picsConfigurator;

static StivaleEnvironment_t _stivaleEnvironment;
static StivaleEnvironment stivaleEnvironment = &_stivaleEnvironment;

static void outputter( RuneString string ) {
    term->PutRuneString( term, string );
}

static void halt( TextTerminal term, RuneString msgFormat, ...) 
{
    if (term && msgFormat && msgFormat[0])
    {
        va_list varargs;
        va_start(varargs, msgFormat);
        term->PutFormattedRuneStringUsingVarargs( term, msgFormat, varargs );
        va_end(varargs);
    }

    for (;;)
        asm("hlt");
}

void _start( Stivale2SystemInformation stivale2SystemInformation )
{
    PopulateStivaleEnvironment( stivaleEnvironment, stivale2SystemInformation );

    Stivale2ResponseTagFramebuffer frameBufferResponseTag = stivaleEnvironment->GetTag( stivaleEnvironment, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID );

    PopulateFrameBuffer( fb, frameBufferResponseTag->framebuffer_width, frameBufferResponseTag->framebuffer_height, frameBufferResponseTag->framebuffer_bpp, (void*)(frameBufferResponseTag->framebuffer_addr) );
    PopulateTextTerminal( term, fb, RetrieveTextTerminalFixedFont8x16() );

    Error e = PopulateCpuInformation(cpuInfo);

    if (e == ErrorFacilityNotPresent)
        halt(term, U"No support for CPUID");

    if (!cpuInfo->CpuSupports(cpuInfo, OnboardAPIC))
        halt(term, U"No onboard APIC present");

    PopulatePICsConfigurator(picsConfigurator);
    picsConfigurator->Reinitalize( 0x20, 0x28 );
    picsConfigurator->Disable();

    PopulateInterruptDescriptorTableBuilder( idtBuilder );
    idtBuilder->InitializeBaseVectorCallback( outputter );
    idtBuilder->ActivateTable();

    term->PutRuneString( term, U"The K-OS!\n" );

    halt( term, U"Halting." );
}
