#include <bus/pci.h>
#include <sys/types.h>
#include <platform/ia-32/asm.h>

char* PCI_CLASS_DESCRIPTIONS[] =
        { "Pre PCI",                  "Mass Storage Controller",          "Network Controller",
          "Display Controller",       "Multimedia Controller",            "Memory Controller",
          "Bridge Device",            "Simple Communication Controller",  "Base System Peripheral",
          "Input Device",             "Docking Station",                  "Processor",
          "Serial Bus Controller",    "Wireless Controller",              "Intellegent I/O Controller",
          "Satellite Controller",     "Encryption/Decryption Controller", "Data Aquisition and Signal Processing Controller"
        };


const char* get_static_pci_class_description( u8 class_number ) {
    if (class_number > 17) {
        return "Unknown";
    }
    else {
        return PCI_CLASS_DESCRIPTIONS[class_number];
    }
}


/* On bus device 'slot' in bus 'bus', execute function 'function' and read return register 'register'
 * (which is in the range 00 .. 3c step 0x4).  Return register word selected.
 */
static inline u32 pci_read_config_register( u8 bus, u8 slot, u8 function, pci_registers read_register ) {
    u32 pci_config_word = 0;

    pci_config_set_enable_bit( pci_config_word );
    pci_config_set_bus_number( pci_config_word, bus );
    pci_config_set_slot_number( pci_config_word, slot );
    pci_config_set_function_number( pci_config_word, function );
    pci_config_set_register_number( pci_config_word, read_register );

    ioport_writel( PCI_CONFIG_ADDRESS_IO_PORT, pci_config_word );
    return ioport_readl( PCI_CONFIG_DATA_IO_PORT );
}


void init_pci_scan( PCI_SCAN_ITERATOR itr ) {
    itr->next_bus_index   = 0;
    itr->next_slot_index  = 0;
}


/* move itr->next_slot_index up one, and if that pushes it past last possible
 * slot in current bus, increment bus.  May increment bus past last possible
 * value */
static inline void increment_pci_iterator( PCI_SCAN_ITERATOR itr ) {
    if (++itr->next_slot_index > 32) {    // 2 ** 5 == 32
        itr->next_slot_index = 0;
        itr->next_bus_index++;
    }
}


pci_device* continue_pci_scan( PCI_SCAN_ITERATOR itr, pci_device* next_device ) {
    u32 pci_register_value;
    u16 vendor, device_id;

    while (itr->next_bus_index <= 0xff) {   // sizeof(bus) is one byte
        pci_register_value = pci_read_config_register( itr->next_bus_index, itr->next_slot_index, 0, DEVICE_AND_VENDOR_IDS );

        if ((u16)pci_register_value != 0xffff) {    // lower 16 bits are vendor id and 0xffff is invalid vendor id
            next_device->bus_number         = itr->next_bus_index;
            next_device->slot_number        = itr->next_slot_index;
            next_device->function_number    = 0;
            next_device->vendor_id          = pci_config_get_vendor_id( pci_register_value );
            next_device->device_id          = pci_config_get_device_id( pci_register_value );

            pci_register_value = pci_read_config_register( itr->next_bus_index, itr->next_slot_index, 0, CLASS_PROG_REVISION );

            next_device->class_code             = pci_config_get_class_code           ( pci_register_value );
            next_device->subclass               = pci_config_get_subclass             ( pci_register_value );
            next_device->programming_interface  = pci_config_get_programming_interface( pci_register_value );
            next_device->revision_id            = pci_config_get_device_revision_id   ( pci_register_value );

            next_device->last_register_value    = pci_register_value;

            increment_pci_iterator( itr );

            return next_device;
        }
        else {
            increment_pci_iterator( itr );
        }
    }

    // ASSERT: entire bus scanned
    next_device = NULL;
    return next_device;
}


inline u16 pci_config_get_device_id             ( u32 pci_register_value ) { return (u16)(pci_register_value >> 16);   }
inline u16 pci_config_get_vendor_id             ( u32 pci_register_value ) { return (u16)pci_register_value;           }
inline u8  pci_config_get_class_code            ( u32 pci_register_value ) { return  (u8)(pci_register_value >> 24);   }
inline u8  pci_config_get_subclass              ( u32 pci_register_value ) { return  (u8)(pci_register_value >> 16);   }
inline u8  pci_config_get_programming_interface ( u32 pci_register_value ) { return  (u8)(pci_register_value >>  8);   }
inline u8  pci_config_get_device_revision_id    ( u32 pci_register_value ) { return  (u8)pci_register_value;           }

