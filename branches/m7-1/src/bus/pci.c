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


//unsigned short pciConfigReadWord (unsigned short bus, unsigned short slot,
u32 pciConfigReadWord (unsigned short bus, unsigned short slot, unsigned short func, unsigned short offset)
 {  
    unsigned long address;
    unsigned long lbus = (unsigned long)bus;
    unsigned long lslot = (unsigned long)slot;
    unsigned long lfunc = (unsigned long)func;
    unsigned short tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (unsigned long)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((u32)0x80000000));

    /* write out the address */
    ioport_writel (0xCF8, address);
    /* read in the data */
//    tmp = (unsigned short)((ioport_readl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return ioport_readl( 0xcfc );
//    return (tmp);
 }

 unsigned short pciCheckVendor(unsigned short bus, unsigned short slot)
 {  
    unsigned short vendor,device;
    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus,slot,0,0)) != 0xFFFF) {
       device = pciConfigReadWord(bus,slot,0,2);
    } return (vendor);
 }


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


/* move itr->next_device_index up one, and if that pushes it past last possible
 * device in current bus, increment bus.  May increment bus past last possible
 * value */
static inline void increment_pci_iterator( PCI_SCAN_ITERATOR itr ) {
    if (++itr->next_device_index > 32) {    // 2 ** 5 == 32
        itr->next_device_index = 0;
        itr->next_bus_index++;
    }
}


pci_device* continue_pci_scan( PCI_SCAN_ITERATOR itr, pci_device* next_device ) {
    u32 pci_register_value;

    if (itr->next_device_index++ < 0x0f) {
        pci_register_value = pciConfigReadWord( itr->next_bus_index, itr->next_device_index, 0, 0 );
        next_device->bus_number = 0;
        next_device->device_number = itr->next_device_index;
        next_device->function_number = 0;
        next_device->vendor_id = (pci_register_value & 0x0000ffff);
        next_device->device_id = (pci_register_value >> 16);
    }
    else {
        next_device = NULL;
    }

    return next_device;

//    while (itr->next_bus_index <= 0xff) {   // sizeof(bus) is one byte
////        pci_register_value = pci_read_config_register( itr->next_bus_index, itr->next_device_index, 0, TYPE00_DEVICE_AND_VENDOR_IDS );
////        pci_register_value = pciCheckVendor( itr->next_bus_index, itr->next_device_index );
//        pci_register_value = pciConfigReadWord( itr->next_bus_index, itr->next_device_index, 0, 0 );
////        if ((pci_register_value & 0x00ff) != 0x00ff) {
//        if (pci_register_value != 0xffff) {
//            next_device->bus_number         = itr->next_bus_index;
//            next_device->device_number      = itr->next_device_index;
//            next_device->function_number    = 0;
//            next_device->vendor_id          = pci_register_value;
//            next_device->device_id          = 0;
//
//            increment_pci_iterator( itr );
//
//            return next_device;
//        }
//        else {
//            increment_pci_iterator( itr );
//        }
//    }

    // ASSERT: entire bus scanned
//    next_device = NULL;
//    return next_device;
}
