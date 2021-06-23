#pragma once

#include <stdint.h>

typedef struct Stivale2Tag_t {
    uint64_t identifier;
    uint64_t next;
} __attribute__((packed)) Stivale2Tag_t, *Stivale2Tag;

/* --- Header --------------------------------------------------------------- */
/*  Information passed from the kernel to the bootloader                      */

typedef struct Stivale2Header_t {
    uint64_t entry_point;
    uint64_t stack;
    uint64_t flags;
    uint64_t tags;
} __attribute__((packed)) Stivale2Header_t, *Stivale2Header;

#define STIVALE2_HEADER_TAG_FRAMEBUFFER_ID 0x3ecc1bc43d0f7971

#define STIVALE2_HEADER_TAG_FB_MTRR_ID 0x4c7bb07731282e00

typedef struct Stivale2RequestTagFramebuffer_t {
    Stivale2Tag_t tag;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
} __attribute__((packed)) Stivale2HeaderTagFramebuffer_t, *Stivale2HeaderTagFramebuffer;

#define STIVALE2_HEADER_TAG_SMP_ID 0x1ab015085f3273df

struct Stivale2RequestTagSmp_t {
    Stivale2Tag_t tag;
    uint64_t flags;
} __attribute__((packed)) Stivale2RequestTagSmp_t, *Stivale2RequestTagSmp;

#define STIVALE2_HEADER_TAG_5LV_PAGING_ID 0x932f477032007e8f

/* --- Struct --------------------------------------------------------------- */
/*  Information passed from the bootloader to the kernel                      */

typedef struct Stivale2SystemInformation_t
{
#define STIVALE2_BOOTLOADER_BRAND_SIZE 64
    char bootloader_brand[STIVALE2_BOOTLOADER_BRAND_SIZE];

#define STIVALE2_BOOTLOADER_VERSION_SIZE 64
    char bootloader_version[STIVALE2_BOOTLOADER_VERSION_SIZE];

    uint64_t tags;
} __attribute__((packed)) Stivale2SystemInformation_t, *Stivale2SystemInformation;

#define STIVALE2_STRUCT_TAG_CMDLINE_ID 0xe5e76a1b4597a781

typedef struct Stivale2ResponseTagCmdline_t {
    Stivale2Tag_t tag;
    uint64_t cmdline;
} __attribute__((packed)) Stivale2ResponseTagCmdline_t, *Stivale2ResponseTagCmdline;

#define STIVALE2_STRUCT_TAG_MEMMAP_ID 0x2187f79e8612de07

enum {
    STIVALE2_MMAP_USABLE = 1,
    STIVALE2_MMAP_RESERVED = 2,
    STIVALE2_MMAP_ACPI_RECLAIMABLE = 3,
    STIVALE2_MMAP_ACPI_NVS = 4,
    STIVALE2_MMAP_BAD_MEMORY = 5,
    STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE = 0x1000,
    STIVALE2_MMAP_KERNEL_AND_MODULES = 0x1001
};

typedef struct Stivale2MmapEntry_t {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
} __attribute__((packed)) Stivale2MmapEntry_t, *Stivale2MmapEntry;

struct Stivale2ResponseTagMemmap_t {
    Stivale2Tag_t tag;
    uint64_t entries;
    Stivale2MmapEntry_t memmap[];
} __attribute__((packed)) Stivale2ResponseTagMemmap_t, *Stivale2ResponseTagMemmap;

#define STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID 0x506461d2950408fa

enum {
    STIVALE2_FBUF_MMODEL_RGB  = 1
};

typedef struct Stivale2ResponseTagFramebuffer_t {
    Stivale2Tag_t tag;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
} __attribute__((packed)) Stivale2ResponseTagFramebuffer_t, *Stivale2ResponseTagFramebuffer;

#define STIVALE2_STRUCT_TAG_FB_MTRR_ID 0x6bc1a78ebe871172

#define STIVALE2_STRUCT_TAG_MODULES_ID 0x4b6fe466aade04ce

typedef struct Stivale2Module_t {
    uint64_t begin;
    uint64_t end;

#define STIVALE2_MODULE_STRING_SIZE 128
    char string[STIVALE2_MODULE_STRING_SIZE];
} __attribute__((packed)) Stivale2Module_t, *Stivale2Module;

typedef struct Stivale2ResponseTagModules_t {
    Stivale2Tag_t tag;
    uint64_t module_count;
    Stivale2Module_t modules[];
} __attribute__((packed)) Stivale2ResponseTagModules_t, *Stivale2ResponseTagModules;

#define STIVALE2_STRUCT_TAG_RSDP_ID 0x9e1786930a375e78

typedef struct Stivale2ResponseTagRsdp_t {
    Stivale2Tag_t tag;
    uint64_t rsdp;
} __attribute__((packed)) Stivale2ResponseTagRsdp_t, *Stivale2ResponseTagRsdp;

#define STIVALE2_STRUCT_TAG_EPOCH_ID 0x566a7bed888e1407

typedef struct Stivale2ResponseTagEpoch_t {
    Stivale2Tag_t tag;
    uint64_t epoch;
} __attribute__((packed)) Stivale2ResponseTagEpoch_t, *Stivale2ResponseTagEpoch;

#define STIVALE2_STRUCT_TAG_FIRMWARE_ID 0x359d837855e3858c

typedef struct Stivale2ResponseTagFirmware_t {
    Stivale2Tag_t tag;
    uint64_t flags;
} __attribute__((packed)) Stivale2ResponseTagFirmware_t, *Stivale2ResponseTagFirmware;

#define STIVALE2_STRUCT_TAG_SMP_ID 0x34d1d96339647025

typedef struct Stivale2SmpInfo_t {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t target_stack;
    uint64_t goto_address;
    uint64_t extra_argument;
} __attribute__((packed)) Stivale2SmpInfo_t, *Stivale2SmpInfo;

typedef struct Stivale2ResponseTagSmp_t {
    Stivale2Tag_t tag;
    uint64_t flags;
    uint32_t bsp_lapic_id;
    uint32_t unused;
    uint64_t cpu_count;
    Stivale2SmpInfo_t smp_info[];
} __attribute__((packed)) Stivale2ResponseTagSmp_t, *Stivale2ResponseTagSmp;

#define STIVALE2_STRUCT_TAG_PXE_SERVER_INFO 0x29d1e96239247032

typedef struct Stivale2ResponseTagPxeServerInfo_t {
    Stivale2Tag_t tag;
    uint32_t server_ip;
} __attribute__((packed)) Stivale2ResponseTagPxeServerInfo_t, *Stivale2ResponseTagPxeServerInfo;

typedef struct StivaleEnvironment_t {
    Stivale2SystemInformation systemInformation;

    // Return a pointer to the tag (there are several values to which it may need to be cast),
    // or null if the tag does not exist.
    void *(*GetTag)(struct StivaleEnvironment_t *environment, uint64_t tagIdentifier);
} StivaleEnvironment_t, *StivaleEnvironment;

void PopulateStivaleEnvironment( StivaleEnvironment environment, Stivale2SystemInformation systemInformation );