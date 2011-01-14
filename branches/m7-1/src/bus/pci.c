#include <bus/pci.h>
#include <sys/types.h>
#include <platform/ia-32/asm.h>

/* On bus device 'device' in bus 'bus', execute function 'function' and read return register 'register'
 * (which is in the range 00 .. 3c step 0x4).  Return register word selected.
 */
static inline u32 pci_read_config_register( u8 bus, u8 device, u8 function, pci_registers read_register ) {
    u32 pci_config_word = 0;

    pci_config_set_enable_bit( pci_config_word );
    pci_config_set_bus_number( pci_config_word, bus );
    pci_config_set_device_number( pci_config_word, device );
    pci_config_set_function_number( pci_config_word, function );
    pci_config_set_register_number( pci_config_word, read_register );

    ioport_writel( PCI_CONFIG_ADDRESS_IO_PORT, pci_config_word );
    return ioport_readl( PCI_CONFIG_DATA_IO_PORT );
}


void init_pci_scan( PCI_SCAN_ITERATOR itr ) {
    itr->next_bus_index     = 0;
    itr->next_device_index  = 0;
}


static inline void increment_pci_iterator( PCI_SCAN_ITERATOR itr ) {
    if (++itr->next_device_index > 32) {    // 2 ** 5 == 32
        itr->next_device_index = 0;
        itr->next_bus_index++;
    }
}


void continue_pci_scan( PCI_SCAN_ITERATOR itr, pci_device* next_device ) {
    u32 pci_register_value = 0xffff;

    while (itr->next_bus_index <= 0xff) {   // sizeof(bus) is one byte
        pci_register_value = pci_read_config_register( itr->next_bus_index, itr->next_device_index, 0, TYPE00_DEVICE_AND_VENDOR_IDS );
        if ((pci_register_value & 0x00ff) != 0x00ff) {
            next_device->bus_number         = itr->next_bus_index;
            next_device->device_number      = itr->next_device_index;
            next_device->function_number    = 0;
            next_device->vendor_id          = (u32)(pci_register_value & 0x00ff);
            next_device->device_id          = (u32)(pci_register_value >> 16);

            increment_pci_iterator( itr );

            return;
        }
        else {
            increment_pci_iterator( itr );
        }
    }

    // ASSERT: entire bus scanned
    next_device = NULL;
    return;
}
