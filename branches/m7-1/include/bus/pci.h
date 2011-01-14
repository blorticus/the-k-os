#ifndef __PCI_H__
#define __PCI_H__

#include <sys/types.h>

// PCI CONFIG_ADDRESS specifies config activity; CONFIG_DATA specifies data written to
// or read from configuration
#define PCI_CONFIG_ADDRESS_IO_PORT  0xCF8
#define PCI_CONFIG_DATA_IO_PORT     0xCFC

/* PCI CONFIG_ADDRESS is essentially a command assertion for PCI.  Structure of this
 * 32-bit word (little endian) is:
 *   BITS       FUNCTION        NOTE
 *   31         enable bit      if config_data should be used
 *   30 - 24    (reserved)
 *   23 - 16    bus number      the number of the PCI bus to access
 *   15 - 11    device number   the number of the PCI device to access on the specified bus
 *   10 -  8    function number the number of the function for the device
 *    7 -  2    register number 32-bit area in the device's configuration space
 *    1 -  0    (zero)
 */

// macros for reading and manipulating 32-bit PCI CONFIG_ADDRESS, which is always little endian
// 'pci_config_word' is u32 and should start as all zero (but pci_config_init can be called if needed to do so)
// if the value provided for any "set" is outside the permissable bounds, the results are undefined
#define pci_config_init( pci_config_word )                                  (pci_config_word = pci_config_word ^ pci_config_word)
#define pci_config_set_enable_bit( pci_config_word )                        (pci_config_word |= 0x80000000)
#define pci_config_unset_enable_bit( pci_config_word )                      (pci_config_word &= 0x7fffffff)
#define pci_config_set_bus_number( pci_config_word, bus_number )            (pci_config_word |= ((u32)bus_number << 23))        // MUST BE >= 0 && < 256 
#define pci_config_set_device_number( pci_config_word, device_number)       (pci_config_word |= ((u32)device_number << 15))     // MUST BE >= 0 && < 32
#define pci_config_set_function_number( pci_config_word, function_number)   (pci_config_word |= ((u32)function_number << 10))   // MUST BE >= 0 && < 8
#define pci_config_set_register_number( pci_config_word, register_number)   (pci_config_word |= ((u32)register_number << 7))    // MUST BE >= 0 && < 64

// PCI class codes and text descriptions
#define PCI_CLASS_PRE_PCI               0x00
#define PCI_CLASS_MASS_STORAGE_CNTRLR   0x01
#define PCI_CLASS_NETWORK_CNTRLR        0x02
#define PCI_CLASS_DISPLAY_CNTRLR        0x03
#define PCI_CLASS_MULTIMEDIA_CNTRLR     0x04
#define PCI_CLASS_MEMORY_CNTRLR         0x05
#define PCI_CLASS_BRIDGE_DEVICE         0x06
#define PCI_CLASS_SIMPLE_COMM_CNTRLR    0x07
#define PCI_CLASS_BASE_SYS_PERIPH       0x08
#define PCI_CLASS_INPUT_DEVICE          0x09
#define PCI_CLASS_DOCKING_STATION       0x0a
#define PCI_CLASS_PROCESSOR             0x0b
#define PCI_CLASS_SERIAL_BUS_CNTRLR     0x0c
#define PCI_CLASS_WIRELESS_CNTRLR       0x0d
#define PCI_CLASS_I_O_CNTRLR            0x0e
#define PCI_CLASS_SAT_COMM_CNTRLR       0x0f
#define PCI_CLASS_ENCRYPTION_CNTRLR     0x10
#define PCI_CLASS_DATA_AQU_SIGNAL_PROC_CNTRLR   0x11
#define PCI_CLASS_NONE                  0xff

char* PCI_CLASS_DESCRIPTIONS[] = {  "Pre PCI",                  "Mass Storage Controller",          "Network Controller",
                                    "Display Controller",       "Multimedia Controller",            "Memory Controller",
                                    "Bridge Device",            "Simple Communication Controller",  "Base System Peripheral",
                                    "Input Device",             "Docking Station",                  "Processor",
                                    "Serial Bus Controller",    "Wireless Controller",              "Intellegent I/O Controller",
                                    "Satellite Controller",     "Encryption/Decryption Controller", "Data Aquisition and Signal Processing Controller"
                                  };

typedef enum pci_registers {
    TYPE00_DEVICE_AND_VENDOR_IDS        = 0x00,
    TYPE00_STATUS_AND_COMMAND           = 0x04,
    TYPE00_CLASS_PROG_REVISION          = 0x08,
    TYPE00_BIST_TYPE_TIMER_CACHE        = 0x0c,
    TYPE00_BAR0                         = 0x10,
    TYPE00_BAR1                         = 0x14,
    TYPE00_BAR2                         = 0x18,
    TYPE00_BAR3                         = 0x1c,
    TYPE00_BAR4                         = 0x20,
    TYPE00_BAR5                         = 0x24,
    TYPE00_CARDBUS_CIS_PTR              = 0x28,
    TYPE00_SUBSYSTEM                    = 0x2c,
    TYPE00_EXPANSION_ROM_BASE_ADDR      = 0x30,
    TYPE00_CAPABILITIES_PTR             = 0x34,
    TYPE00_LATENCY_GRANT_INTERRUPT      = 0x3c
} pci_registers;


typedef struct pci_scan_iterator {
    u32 next_bus_index;
    u32 next_device_index;
} * PCI_SCAN_ITERATOR;


typedef struct pci_device {
    u8 bus_number;
    u8 device_number;
    u8 function_number;
    u8 vendor_id;
    u8 device_id;
} pci_device;


// To scan the PCI bus, call 'init_oci_scan' and provide offset struct.  Call 'continue_pci_scan' on the offset struct.
// When this method encounters a device, it fills in 'next_device' and updates 'offset'.  Keep calling with updated
// 'offset' until 'next_device' is NULL, meaning the end of the bus has been reached.
void init_pci_scan    ( PCI_SCAN_ITERATOR itr );
void continue_pci_scan( PCI_SCAN_ITERATOR itr, pci_device* next_device );


#endif
