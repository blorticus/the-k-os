# Journal

## 23 Apr 2021

I decided to start over creating this OS.  Completely from scratch.  I decided to do it quickly and iteratively, without over-architecting.

First, I build a Linux VM to run qemu.  I am using Visual Studio Code.  It'll probably be cranky because the application won't have normal stdlibs, but I've grown fond of Visual Studio Code as an environment.

## 26 Apr 2021

The multiboot2 spec is a mess and using grub for this kind of bootloading is complicated.  I found limine, and I'm going to give that a try.

## 01 May 2021

Having issues with the build of limine using the cross-compiler.  Realized that I built with 32-bit (i686-elf).  Build x86_64-elf and now it compiles limine just fine.

Thoughts about the basic terminal functions:

- separate FrameBuffer routines from text writing routines on the FrameBuffer
- polymorphism by using function references on structs (has the disadvantage of needing to pass a pointer to the struct manually)

Some initial thoughts:

```c
typedef uint32_t FrameBufferColor;

enum FrameBufferAbstractPaletteColor{
    FBAP_White, FBAP_Black, FBAP_Red, FBAP_Blue, FBAP_Green
};

typedef struct FrameBuffer_t {
    uint BitsPerPixel;  // generally 8, 16 or 24
    uint HorizontalPixels;
    uint VerticalPixels;
    uint* (DrawPixelAt)(uint row, uint column, FrameBufferColor color);
    FrameBufferColor (GenerateConcreteColorFromAbstractPalette)( FrameBufferAbstractPaletteColor color );
} *FrameBuffer;

static FrameBuffer_t fb;

Populate24BitFrameBuffer( FrameBuffer fb, uint hrez, uint vrez, uint bpp, void* frameBufferStart );

FrameBufferColor white = fb->GenerateConcreteColorFromAbstractPalette( FBAP_White );

for (uint row = 10; row > 20; row++)
    for (uint col = 10; col < 20; col++)
        fb->DrawPixelAt( row, col, white );
```

## 10 May 2021

I've been working over the last week on getting the FrameBuffer working.  As a
side-effect, I built a simplistic assertion based unit testing suite.  It
allowed me to debug some rendering routines.  I am now switching to building a
basic TextTerminal that uses the FrameBuffer.

And completed write of a single letter (A) in the terminal.

## 4 June 2021

I've completed the StringFormatter and its attachment to the TextTerminal.
Now working on CPUID functions.  All of this is working toward discovery of
the APIC.